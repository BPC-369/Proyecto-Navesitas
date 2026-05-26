// Copyright Epic Games, Inc. All Rights Reserved.

#include "GalagaModificadoMacPawn.h"
#include "GalagaModificadoMacProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "ComponenteCombate.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

const FName AGalagaModificadoMacPawn::MoveForwardBinding("MoveForward");
const FName AGalagaModificadoMacPawn::MoveRightBinding("MoveRight");
const FName AGalagaModificadoMacPawn::FireForwardBinding("FireForward");
const FName AGalagaModificadoMacPawn::FireRightBinding("FireRight");
const FName AGalagaModificadoMacPawn::MoveUpBinding("MoveUp"); //Implementamos el eje Z

AGalagaModificadoMacPawn::AGalagaModificadoMacPawn()
{
	RopaNave = nullptr;
	RopaCubo = nullptr;

	// 1. EL CAMBIO CLAVE: Ajustamos la cápsula que ya viene con el ACharacter
	GetCapsuleComponent()->InitCapsuleSize(60.f, 60.f);
	GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	// Create the mesh component y lo pegamos a la cápsula
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMeshComponent->SetupAttachment(GetCapsuleComponent());
	ShipMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	// Aqui cargamos las 2 mallas que tendra la nave
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaNave(TEXT("StaticMesh'/Game/Geometry/pawn/pawn06.pawn06'"));
	if (FormaNave.Succeeded()) RopaNave = FormaNave.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCubo(TEXT("StaticMesh'/Game/Geometry/pawn/pawn09.pawn09'"));
	if (FormaCubo.Succeeded()) RopaCubo = FormaCubo.Object;

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara3D"));
	CameraBoom->SetupAttachment(RootComponent);// enganchamos la camara a la nave
	CameraBoom->SetUsingAbsoluteRotation(false);// hacemos que la camara rote junto con la nave
	CameraBoom->TargetArmLength = 250.f;// distancia de la camara a la nave
	CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));//inclinamos la camara 
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);//la posicion que estara la camara
	CameraBoom->bDoCollisionTest = true; // Que choque con los edificios si retrocedes
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TerceraPersonaCamera"));

	//enganchamos la camara en el limite de distancia 
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	//nos aseguramos que no gire a lo loco si no que gire con la nave
	CameraComponent->bUsePawnControlRotation = false;
	CameraBoom->bUsePawnControlRotation = false;

	ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));

	// Movement & Weapon
	MoveSpeed = 1000.0f;
	GunOffset = FVector(130.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	bEstaDisparando = false;

	MultiplicadorDanio = 1.0f;
	TiempoDisparoCuadruple = 0.0f;
	BombasRacimoRestantes = 0;
	TiempoBuffoNave = 0.0f;
	VelocidadOriginalNave = 1000.0f;
	TiempoBuffoRobot = 0.0f;
	TiempoCortesDistancia = 0.0f;
	TiempoInmunidad = 0.0f;
	
	//indicamos que la camara gire con la nave y no al reves
	bUseControllerRotationPitch = false;	// Permite que la nave levante/baje la nariz con el ratón
	bUseControllerRotationYaw = false;		// Permite que la nave gire a los lados con el ratón
	bUseControllerRotationRoll = false;		// para que la nave no se incline x si sola

	//ConvertirEnNave();
	EstadoActual = new FEstadoNaveVoladora();

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Jugador");
	}
}

void AGalagaModificadoMacPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);//
	PlayerInputComponent->BindAxis(MoveUpBinding); // Conectamos el eje Z
	PlayerInputComponent->BindAxis(FireRightBinding);

	// agregamos una accion a cada boton agregado 
	//para cambiar entre mallas
	PlayerInputComponent->BindAction("BotonCambio", IE_Pressed, this, &AGalagaModificadoMacPawn::Transformar);
	// para detectar el disparo
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// para detectar el movimiento del mouse en ambos ejes X e Y
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	// para empezar el disparo al presionar el boton
	PlayerInputComponent->BindAction("Disparar", IE_Pressed, this, &AGalagaModificadoMacPawn::EmpezarDisparo);
	PlayerInputComponent->BindAction("Disparar", IE_Released, this, &AGalagaModificadoMacPawn::DetenerDisparo);
}

void AGalagaModificadoMacPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float ForwardValue = GetInputAxisValue(MoveForwardBinding); // adelante/atrás
	const float RightValue = GetInputAxisValue(MoveRightBinding);// izquierda/derecha
	const float UpValue = GetInputAxisValue(MoveUpBinding); // arriba/abajo

	const float ValorMouseX = GetInputAxisValue("Turn"); 
	const float ValorMouseY = GetInputAxisValue("LookUp");

	if (CameraBoom != nullptr && (ValorMouseX != 0.0f || ValorMouseY != 0.0f))
	{
		// Creamos la rotación con una sensibilidad de 3.0f
		// Pitch (Y) levanta/baja la nariz. Yaw (X) gira a izquierda/derecha. Roll (Z) lo dejamos en 0.
		FRotator RotacionNave = FRotator(ValorMouseY * 3.0f, ValorMouseX * 3.0f, 0.0f);

		// Como usamos "LocalRotation", la nave girará sobre su propio eje.
		AddActorLocalRotation(RotacionNave);
	}

	// EL CAMBIO MAESTRO: Usar el CharacterMovement
	// Esto aplica las velocidades de MoveSpeed automáticamente y calcula las colisiones por ti.
	AddMovementInput(GetActorForwardVector(), ForwardValue);
	AddMovementInput(GetActorRightVector(), RightValue);
	AddMovementInput(GetActorUpVector(), UpValue);

	if (bEstaDisparando)
	{
		Disparar();
		// Nota: Como tu función FireShot ya tiene el "if (bCanFire == true)", 
		// esto es súper seguro. Solo disparará cuando el FireRate lo permita.
	}

	if (TiempoDisparoCuadruple > 0.0f) TiempoDisparoCuadruple -= DeltaSeconds;

	if (TiempoCortesDistancia > 0.0f) TiempoCortesDistancia -= DeltaSeconds;

	if (TiempoInmunidad > 0.0f)
	{
		TiempoInmunidad -= DeltaSeconds;
		if (TiempoInmunidad <= 0.0f) MultiplicadorDanio = 1.0f; // Pierde el doble daño
	}

	if (TiempoBuffoNave > 0.0f)
	{
		TiempoBuffoNave -= DeltaSeconds;
		if (TiempoBuffoNave <= 0.0f)
		{
			// Se acaba el buffo, regresamos a la normalidad
			MoveSpeed = VelocidadOriginalNave;
			GetCharacterMovement()->MaxFlySpeed = MoveSpeed;
			MultiplicadorDanio = 1.0f;
		}
	}

	if (TiempoBuffoRobot > 0.0f)
	{
		TiempoBuffoRobot -= DeltaSeconds;
		if (TiempoBuffoRobot <= 0.0f)
		{
			MoveSpeed = 300.0f; // Tu velocidad normal de robot
			GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
			// Aquí desactivarías tu lógica de Dash reducido
		}
	}
}

void AGalagaModificadoMacPawn::BeginPlay()
{
	Super::BeginPlay();

	// Aquí SÍ se aplican correctamente los modos de vuelo y caminata del motor
	ConvertirEnNave();
}

void AGalagaModificadoMacPawn::FireShot(FVector FireDirection)
{
	if (bCanFire == true)
	{
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			UWorld* const World = GetWorld();

			if (World != nullptr)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;

				if (BombasRacimoRestantes > 0)
				{
					// EFECTO RACIMO
					BombasRacimoRestantes--;
					for (int i = 0; i < 8; i++)
					{
						FRotator RacimoRot = FireRotation;
						RacimoRot.Yaw += (45.0f * i);
						World->SpawnActor<AGalagaModificadoMacProjectile>(GetActorLocation(), RacimoRot, SpawnParams);
					}
				}
				else if (TiempoDisparoCuadruple > 0.0f)
				{
					// EFECTO CUÁDRUPLE
					float Sep1 = 15.0f;
					float Sep2 = 45.0f;

					FVector Izq1 = GetActorLocation() + FireRotation.RotateVector(FVector(GunOffset.X, -Sep1, GunOffset.Z));
					FVector Der1 = GetActorLocation() + FireRotation.RotateVector(FVector(GunOffset.X, Sep1, GunOffset.Z));
					FVector Izq2 = GetActorLocation() + FireRotation.RotateVector(FVector(GunOffset.X, -Sep2, GunOffset.Z));
					FVector Der2 = GetActorLocation() + FireRotation.RotateVector(FVector(GunOffset.X, Sep2, GunOffset.Z));

					World->SpawnActor<AGalagaModificadoMacProjectile>(Izq1, FireRotation, SpawnParams);
					World->SpawnActor<AGalagaModificadoMacProjectile>(Der1, FireRotation, SpawnParams);
					World->SpawnActor<AGalagaModificadoMacProjectile>(Izq2, FireRotation, SpawnParams);
					World->SpawnActor<AGalagaModificadoMacProjectile>(Der2, FireRotation, SpawnParams);
				}
				else
				{
					// DISPARO NORMAL (Doble Cañón Original)
					float SeparacionCanones = 23.0f;
					FVector OffsetIzquierdo = FVector(GunOffset.X, -SeparacionCanones, GunOffset.Z);
					FVector OffsetDerecho = FVector(GunOffset.X, SeparacionCanones, GunOffset.Z);
					FVector SpawnLocationIzquierdo = GetActorLocation() + FireRotation.RotateVector(OffsetIzquierdo);
					FVector SpawnLocationDerecho = GetActorLocation() + FireRotation.RotateVector(OffsetDerecho);

					World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocationIzquierdo, FireRotation, SpawnParams);
					World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocationDerecho, FireRotation, SpawnParams);
				}

				bCanFire = false;
				World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AGalagaModificadoMacPawn::ShotTimerExpired, FireRate);

				if (FireSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
				}
			}
		}
	}
}

void AGalagaModificadoMacPawn::ShotTimerExpired()
{
	bCanFire = true;
}

void AGalagaModificadoMacPawn::Disparar()
{
	// Le pasamos el mensaje de atacar a la respectiva forma ya sea nave o robot
	// y el mensaje se encargara de decidir si se dispara o no dependiendo de la forma actual
	if (EstadoActual != nullptr)
	{
		EstadoActual->EjecutarAtaque(this);
	}
}

void AGalagaModificadoMacPawn::EmpezarDisparo()
{
	bEstaDisparando = true;
	Disparar(); // Disparamos una vez inmediatamente para que sea responsivo al primer clic
}

void AGalagaModificadoMacPawn::DetenerDisparo()
{
	bEstaDisparando = false;
}

// --- HERRAMIENTAS PARA EL PATRÓN STATE ---
void AGalagaModificadoMacPawn::CambiarEstado(IEstadoNave* NuevoEstado)
{
	// Si teniamos la otra forma(ejem Nave), la eliminamos para evitar fugas de memoria
	if (EstadoActual != nullptr)
	{
		delete EstadoActual;
	}
	// Nos colocamos la nueva Forma(ejem Robot) y vicebersa
	EstadoActual = NuevoEstado;
}

void AGalagaModificadoMacPawn::ConvertirEnNave()
{
	// Le ponemos la malla de nave al peón junto con sus stats correspondientes
	if (RopaNave != nullptr) ShipMeshComponent->SetStaticMesh(RopaNave);

	// Activamos el modo VUELO (Gravedad = 0, Movimiento Libre en Z)
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void AGalagaModificadoMacPawn::ConvertirEnRobot()
{
	// Le ponemos la malla de cubo al peón junto con sus stats correspondientes
	if (RopaCubo != nullptr) ShipMeshComponent->SetStaticMesh(RopaCubo);

	// Activamos el modo CAMINAR (Le afecta la gravedad, cae al suelo)
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void FEstadoNaveVoladora::EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto)
{
	// En estado de Nave le ordenamos al pawn que se ponga la malla de cubo (Robot)
	NaveContexto->ConvertirEnRobot();

	// Le ordenamos al pawn que cambie su cerebro actual al de Robot
	NaveContexto->CambiarEstado(new FEstadoNaveRobot());
}

void FEstadoNaveRobot::EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto)
{
	// En forma Robot le ordenamos al pawn que vuelva a su malla original
	NaveContexto->ConvertirEnNave();

	// Le ordenamos al pawn que recupere el cerebro de vuelo
	NaveContexto->CambiarEstado(new FEstadoNaveVoladora());
}

void AGalagaModificadoMacPawn::Transformar()
{
	// dependiendo de la forma actual, le ordenamos al pawn que se transforme a la otra forma
	if (EstadoActual != nullptr)
	{
		EstadoActual->EjecutarTransformacion(this);
	}
}

void FEstadoNaveVoladora::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto)
{
	// cuando EstadoActual = FEstadoNaveVoladora, le ordenamos al pawn que dispare hacia adelante
	NaveContexto->FireShot(NaveContexto->GetActorForwardVector());
}

void FEstadoNaveRobot::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto)
{
	// El robot NO tiene cañones. No hacemos absolutamente nada.
	// Opcional: Ponemos un mensaje en pantalla para que sepas que el bloqueo funciona.
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("¡Los robots no pueden disparar!"));
}

float AGalagaModificadoMacPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (TiempoInmunidad > 0.0f) return 0.0f;
	// Recibimos el impacto base del motor de Unreal
	float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Se lo pasamos a tu Chip para que reste la vida, rompa escudos y maneje la lógica
	if (ComponenteCombate != nullptr)
	{
		DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);
	}

	return DanioReal;
}