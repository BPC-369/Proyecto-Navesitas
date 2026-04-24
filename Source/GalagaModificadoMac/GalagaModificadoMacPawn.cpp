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

const FName AGalagaModificadoMacPawn::MoveForwardBinding("MoveForward");
const FName AGalagaModificadoMacPawn::MoveRightBinding("MoveRight");
const FName AGalagaModificadoMacPawn::FireForwardBinding("FireForward");
const FName AGalagaModificadoMacPawn::FireRightBinding("FireRight");
const FName AGalagaModificadoMacPawn::MoveUpBinding("MoveUp"); //Implementamos el eje Z

AGalagaModificadoMacPawn::AGalagaModificadoMacPawn()
{
	RopaNave = nullptr;
	RopaCubo = nullptr;

	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetNotifyRigidBodyCollision(true);
	// 2. Nos aseguramos de que devuelva golpes (Bloquee objetos dinámicos)
	ShipMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Aqui cargamos las 2 mallas que tendra la nave
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaNave(TEXT("StaticMesh'/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO'"));
	if (FormaNave.Succeeded()) RopaNave = FormaNave.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCubo(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (FormaCubo.Succeeded()) RopaCubo = FormaCubo.Object;

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara3D"));
	CameraBoom->SetupAttachment(RootComponent);// enganchamos la camara a la nave
	CameraBoom->SetUsingAbsoluteRotation(false);// hacemos que la camara rote junto con la nave
	CameraBoom->TargetArmLength = 200.f;// distancia de la camara a la nave
	CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));//inclinamos la camara 
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);//la posicion que estara la camara
	CameraBoom->bDoCollisionTest = true; // Que choque con los edificios si retrocedes
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TerceraPersonaCamera"));

	//enganchamos la camara en el limite de distancia 
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	//nos aseguramos que no gire a lo loco si no que gire con la nave
	CameraComponent->bUsePawnControlRotation = false;
	CameraBoom->bUsePawnControlRotation = false;

	// Movement & Weapon
	MoveSpeed = 1000.0f;
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	VidaMaxima = 500.0f;
	VidaActual = VidaMaxima;
	
	//indicamos que la camara gire con la nave y no al reves
	bUseControllerRotationPitch = false;	// Permite que la nave levante/baje la nariz con el ratón
	bUseControllerRotationYaw = false;		// Permite que la nave gire a los lados con el ratón
	bUseControllerRotationRoll = false;		// para que la nave no se incline x si sola

	ConvertirEnNave();
	EstadoActual = new FEstadoNaveVoladora();

	Faccion = FName("Jugador");
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
	PlayerInputComponent->BindAction("Disparar", IE_Pressed, this, &AGalagaModificadoMacPawn::Disparar);
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

	// Creamos un vector 3D real
	const FVector MoveDirection = FVector(ForwardValue, RightValue, UpValue).GetClampedToMaxSize(1.0f);
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// Movemos la nave
	if (Movement.SizeSquared() > 0.0f)
	{
		if (RootComponent == nullptr) return;
		
		// El hit se usa para detectar colisiones. Si nos chocamos con algo, el valor de "Hit" se actualizará con la información del choque.
		FHitResult Hit(1.f);

		// AddLocalOffset mueve a la nave basándose en hacia dónde está mirando.
		// Si miras al norte y presionas Derecha, te deslizas al este sin girar la nariz de la nave.
		RootComponent->AddLocalOffset(Movement, true, &Hit);

		if (Hit.IsValidBlockingHit())// si nos chocamos con algo nos deslizamos sobre la superficie
		{
			const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
			const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
			RootComponent->AddLocalOffset(Deflection, true);
		}
	}
}

void AGalagaModificadoMacPawn::FireShot(FVector FireDirection)
{
	if (bCanFire == true) // Si podemos disparar, entonces disparamos
	{
		if (FireDirection.SizeSquared() > 0.0f) // Si el vector(Direccion) de disparo es válido, entonces disparamos
		{
			// convertimos la direccion del disparo en una rotacion para que la bala sepa a donde ir
			const FRotator FireRotation = FireDirection.Rotation();
			
			// Calculamos la posicion de la bala con un espaciado dado por el GunOffset para que no salga del centro de la nave
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld(); // Obtenemos el mundo para poder crear la bala en el mundo

			if (World != nullptr) // Si el mundo existe, entonces podemos crear la bala
			{
				FActorSpawnParameters SpawnParams; // Creamos los parametros de spawn para configurar la bala
				SpawnParams.Owner = this; // Indicamos que esta nave es el dueno de la bala

				// creamos la bala en la posicion y rotacion indicados con los parametros de spawn dados
				World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocation, FireRotation, SpawnParams);

				bCanFire = false;
				// Indicamos que no se puede disparar hasta que el timer expire, para controlar la cadencia de disparo
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
	// ¡Aquí en el futuro podrías poner: MoveSpeed = 1000.0f; !
}

void AGalagaModificadoMacPawn::ConvertirEnRobot()
{
	// Le ponemos la malla de cubo al peón junto con sus stats correspondientes
	if (RopaCubo != nullptr) ShipMeshComponent->SetStaticMesh(RopaCubo);
	// ¡Aquí en el futuro podrías poner: MoveSpeed = 300.0f; !
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