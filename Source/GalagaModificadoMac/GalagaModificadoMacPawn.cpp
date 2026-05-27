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
#include "Blueprint/UserWidget.h"			// <- Necesario para CreateWidget
#include "GameFramework/PlayerController.h"	// <- Necesario para FInputModeUIOnly

const FName AGalagaModificadoMacPawn::MoveForwardBinding("MoveForward");
const FName AGalagaModificadoMacPawn::MoveRightBinding("MoveRight");
const FName AGalagaModificadoMacPawn::FireForwardBinding("FireForward");
const FName AGalagaModificadoMacPawn::FireRightBinding("FireRight");
const FName AGalagaModificadoMacPawn::MoveUpBinding("MoveUp");

AGalagaModificadoMacPawn::AGalagaModificadoMacPawn()
{
	RopaNave = nullptr;
	RopaCubo = nullptr;

	GetCapsuleComponent()->InitCapsuleSize(60.f, 60.f);
	GetCapsuleComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ShipMeshComponent->SetupAttachment(GetCapsuleComponent());
	ShipMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaNave(TEXT("StaticMesh'/Game/Geometry/pawn/pawn06.pawn06'"));
	if (FormaNave.Succeeded()) RopaNave = FormaNave.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCubo(TEXT("StaticMesh'/Game/Geometry/pawn/pawn09.pawn09'"));
	if (FormaCubo.Succeeded()) RopaCubo = FormaCubo.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara3D"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(false);
	CameraBoom->TargetArmLength = 250.f;
	CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);
	CameraBoom->bDoCollisionTest = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TerceraPersonaCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	CameraBoom->bUsePawnControlRotation = false;

	ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));

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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	EstadoActual = new FEstadoNaveVoladora();

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Jugador");
	}

	// Inicialización añadida para el Game Over
	bMuerto = false;
	WidgetGameOverClass = nullptr;
}

void AGalagaModificadoMacPawn::BeginPlay()
{
	Super::BeginPlay();
	ConvertirEnNave();
}

void AGalagaModificadoMacPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(MoveUpBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);

	PlayerInputComponent->BindAction("BotonCambio", IE_Pressed, this, &AGalagaModificadoMacPawn::Transformar);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Disparar", IE_Pressed, this, &AGalagaModificadoMacPawn::EmpezarDisparo);
	PlayerInputComponent->BindAction("Disparar", IE_Released, this, &AGalagaModificadoMacPawn::DetenerDisparo);
}

void AGalagaModificadoMacPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);
	const float UpValue = GetInputAxisValue(MoveUpBinding);

	const float ValorMouseX = GetInputAxisValue("Turn");
	const float ValorMouseY = GetInputAxisValue("LookUp");

	if (CameraBoom != nullptr && (ValorMouseX != 0.0f || ValorMouseY != 0.0f))
	{
		FRotator RotacionNave = FRotator(ValorMouseY * 3.0f, ValorMouseX * 3.0f, 0.0f);
		AddActorLocalRotation(RotacionNave);
	}

	AddMovementInput(GetActorForwardVector(), ForwardValue);
	AddMovementInput(GetActorRightVector(), RightValue);
	AddMovementInput(GetActorUpVector(), UpValue);

	if (bEstaDisparando && bCanFire)
	{
		// Solo le pedimos al Pawn que inicie la secuencia de disparo hacia adelante
		FireShot(GetActorForwardVector());
	}

	if (TiempoDisparoCuadruple > 0.0f) TiempoDisparoCuadruple -= DeltaSeconds;
	if (TiempoCortesDistancia > 0.0f) TiempoCortesDistancia -= DeltaSeconds;

	if (TiempoInmunidad > 0.0f)
	{
		TiempoInmunidad -= DeltaSeconds;
		if (TiempoInmunidad <= 0.0f) MultiplicadorDanio = 1.0f;
	}

	if (TiempoBuffoNave > 0.0f)
	{
		TiempoBuffoNave -= DeltaSeconds;
		if (TiempoBuffoNave <= 0.0f)
		{
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
			MoveSpeed = 300.0f;
			GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
		}
	}
}

void AGalagaModificadoMacPawn::FireShot(FVector FireDirection)
{
	if (EstadoActual != nullptr)
	{
		EstadoActual->EjecutarAtaque(this, FireDirection);
	}

	// 2. GESTIONAMOS EL REARME DEL ARMA
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AGalagaModificadoMacPawn::ShotTimerExpired, FireRate);

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void AGalagaModificadoMacPawn::ShotTimerExpired()
{
	bCanFire = true;
}

void AGalagaModificadoMacPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// 1. Verificamos si estamos caminando (Forma Robot)
	if (GetCharacterMovement() && GetCharacterMovement()->MovementMode == MOVE_Walking)
	{
		// 2. Nos aseguramos de no hacernos daño a nosotros mismos
		if (OtherActor != nullptr && OtherActor != this)
		{
			// 3. Comprobamos si el objeto chocado tiene tu Componente de Combate y es enemigo
			UComponenteCombate* CompEnemigo = OtherActor->FindComponentByClass<UComponenteCombate>();

			if (CompEnemigo != nullptr && CompEnemigo->Faccion == FName("Enemigo"))
			{
				float DanioPorChoque = 15.0f; // Puedes ajustar este valor

				// Aplicamos el daño oficial de Unreal
				UGameplayStatics::ApplyDamage(
					OtherActor,
					DanioPorChoque * MultiplicadorDanio,
					GetController(),
					this,
					UDamageType::StaticClass()
				);

				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("¡EMBESTIDA ROBOT!"));
			}
		}
	}
}

void AGalagaModificadoMacPawn::EmpezarDisparo()
{
	bEstaDisparando = true;
}

void AGalagaModificadoMacPawn::DetenerDisparo()
{
	bEstaDisparando = false;
}

void AGalagaModificadoMacPawn::CambiarEstado(IEstadoNave* NuevoEstado)
{
	if (EstadoActual != nullptr)
	{
		delete EstadoActual;
	}
	EstadoActual = NuevoEstado;
}

void AGalagaModificadoMacPawn::ConvertirEnNave()
{
	if (RopaNave != nullptr) ShipMeshComponent->SetStaticMesh(RopaNave);
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void AGalagaModificadoMacPawn::ConvertirEnRobot()
{
	if (RopaCubo != nullptr) ShipMeshComponent->SetStaticMesh(RopaCubo);
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AGalagaModificadoMacPawn::Transformar()
{
	if (EstadoActual != nullptr)
	{
		EstadoActual->EjecutarTransformacion(this);
	}
}

float AGalagaModificadoMacPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (TiempoInmunidad > 0.0f) return 0.0f;

	float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ComponenteCombate != nullptr)
	{
		DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);

		// Lógica de muerte y Game Over
		if (!bMuerto && ComponenteCombate->VidaActual <= 0.0f)
		{
			bMuerto = true;
			ManejarMuerte();
		}
	}

	return DanioReal;
}

void AGalagaModificadoMacPawn::ManejarMuerte()
{
	// 1. Pausar el juego
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// 2. Configurar el modo de entrada para la UI y mostrar el cursor
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	// 3. Crear y mostrar el widget de Game Over
	if (WidgetGameOverClass)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetGameOverClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}
	}
}

void FEstadoNaveVoladora::EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto)
{
	NaveContexto->ConvertirEnRobot();
	NaveContexto->CambiarEstado(new FEstadoNaveRobot());
}

void FEstadoNaveVoladora::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	UWorld* const World = NaveContexto->GetWorld();
	if (World != nullptr)
	{
		const FRotator FireRotation = FireDirection.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = NaveContexto;

		float SeparacionCanones = 23.0f;
		FVector OffsetIzquierdo = FVector(NaveContexto->GunOffset.X, -SeparacionCanones, NaveContexto->GunOffset.Z);
		FVector OffsetDerecho = FVector(NaveContexto->GunOffset.X, SeparacionCanones, NaveContexto->GunOffset.Z);

		FVector SpawnLocationIzquierdo = NaveContexto->GetActorLocation() + FireRotation.RotateVector(OffsetIzquierdo);
		FVector SpawnLocationDerecho = NaveContexto->GetActorLocation() + FireRotation.RotateVector(OffsetDerecho);

		World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocationIzquierdo, FireRotation, SpawnParams);
		World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocationDerecho, FireRotation, SpawnParams);
	}
}

void FEstadoNaveRobot::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection	)
{
	UWorld* const World = NaveContexto->GetWorld();
	if (World != nullptr)
	{
		// 1. Calculamos dónde ocurrirá el corte (150 unidades por delante del robot)
		FVector PosicionGolpe = NaveContexto->GetActorLocation() + (FireDirection * 150.0f);
		float RadioGolpe = 200.0f; // El alcance de tu espada/corte
		float DanioBase = 25.0f;

		// 2. Preparamos los filtros de colisión
		TArray<FOverlapResult> EnemigosGolpeados;
		FCollisionQueryParams ParametrosColision;
		ParametrosColision.AddIgnoredActor(NaveContexto); // El robot es inmune a su propio corte

		// 3. Creamos una esfera invisible que detecta todo lo que toca en ese milisegundo
		bool bHuboGolpe = World->OverlapMultiByChannel(
			EnemigosGolpeados,
			PosicionGolpe,
			FQuat::Identity,
			ECollisionChannel::ECC_Pawn, // Solo buscamos otros Pawns/Personajes
			FCollisionShape::MakeSphere(RadioGolpe),
			ParametrosColision
		);

		// 4. Si la espada tocó algo, le restamos vida
		if (bHuboGolpe)
		{
			for (FOverlapResult& Overlap : EnemigosGolpeados)
			{
				AActor* ActorGolpeado = Overlap.GetActor();
				if (ActorGolpeado != nullptr)
				{
					// Verificamos que sea un enemigo para no golpear aliados o powerups por error
					UComponenteCombate* CompEnemigo = ActorGolpeado->FindComponentByClass<UComponenteCombate>();
					if (CompEnemigo != nullptr && CompEnemigo->Faccion == FName("Enemigo"))
					{
						UGameplayStatics::ApplyDamage(
							ActorGolpeado,
							DanioBase * NaveContexto->MultiplicadorDanio,
							NaveContexto->GetController(),
							NaveContexto,
							UDamageType::StaticClass()
						);
					}
				}
			}
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("¡CORTE FRONTAL!"));
		}
	}
}

void FEstadoNaveRobot::EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto)
{
	NaveContexto->ConvertirEnNave();
	NaveContexto->CambiarEstado(new FEstadoNaveVoladora());
}

// PATRON DECORATOR
FDecoradorRecuperacionNave::FDecoradorRecuperacionNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	if (Contexto && Contexto->ComponenteCombate) {
		Contexto->ComponenteCombate->VidaActual = Contexto->ComponenteCombate->VidaMaxima;
		Contexto->ComponenteCombate->EscudoActual = Contexto->ComponenteCombate->EscudoMaximo;
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("NAVE RECUPERO VIDA Y ESCUDO"));
	}
}

FDecoradorCuadrupleCanon::FDecoradorCuadrupleCanon(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	Contexto->TiempoDisparoCuadruple = 10.0f;
}

void FDecoradorCuadrupleCanon::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	// Si el buffo sigue activo...
	if (NaveContexto->TiempoDisparoCuadruple > 0.0f)
	{
		UWorld* const World = NaveContexto->GetWorld();
		if (World != nullptr)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = NaveContexto;

			float Sep1 = 15.0f;
			float Sep2 = 45.0f;

			FVector Izq1 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, -Sep1, NaveContexto->GunOffset.Z));
			FVector Der1 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, Sep1, NaveContexto->GunOffset.Z));
			FVector Izq2 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, -Sep2, NaveContexto->GunOffset.Z));
			FVector Der2 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, Sep2, NaveContexto->GunOffset.Z));

			World->SpawnActor<AGalagaModificadoMacProjectile>(Izq1, FireRotation, SpawnParams);
			World->SpawnActor<AGalagaModificadoMacProjectile>(Der1, FireRotation, SpawnParams);
			World->SpawnActor<AGalagaModificadoMacProjectile>(Izq2, FireRotation, SpawnParams);
			World->SpawnActor<AGalagaModificadoMacProjectile>(Der2, FireRotation, SpawnParams);

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("4 ES MEJOR QUE 2"));
		}
	}
	else
	{
		// Si se acabó el tiempo, delegamos el disparo al estado base (disparo de 2 balas)
		FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("4 ES MEJOR QUE 2"));
	// (Aquí irá la lógica futura de spawnear las 3 balas adicionales)
}

FDecoradorBombasRacimo::FDecoradorBombasRacimo(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	Contexto->BombasRacimoRestantes += 6;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("BOMBITAS"));
}

void FDecoradorBombasRacimo::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	// Si aún nos queda munición...
	if (NaveContexto->BombasRacimoRestantes > 0)
	{
		UWorld* const World = NaveContexto->GetWorld();
		if (World != nullptr)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = NaveContexto;

			NaveContexto->BombasRacimoRestantes--; // Restamos una bomba

			// Generamos las 8 balas en círculo
			for (int i = 0; i < 8; i++)
			{
				FRotator RacimoRot = FireRotation;
				RacimoRot.Yaw += (45.0f * i);
				World->SpawnActor<AGalagaModificadoMacProjectile>(NaveContexto->GetActorLocation(), RacimoRot, SpawnParams);
			}
		}
	}
	else
	{
		// Si no hay munición, delegamos al disparo normal
		FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
	}
}

FDecoradorSuperBuffoNave::FDecoradorSuperBuffoNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	Contexto->VelocidadOriginalNave = Contexto->MoveSpeed;
	Contexto->MoveSpeed *= 2.0f;
	Contexto->GetCharacterMovement()->MaxFlySpeed = Contexto->MoveSpeed;
	Contexto->MultiplicadorDanio = 1.5f;
	Contexto->TiempoBuffoNave = 8.0f;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("NUESTRO VERDADERO POTENCIAL"));
}

FDecoradorVelocidadDash::FDecoradorVelocidadDash(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	Contexto->MoveSpeed = 800.0f;
	Contexto->GetCharacterMovement()->MaxWalkSpeed = Contexto->MoveSpeed;
	Contexto->TiempoBuffoRobot = 10.0f;
}

FDecoradorCortesDistancia::FDecoradorCortesDistancia(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	Contexto->TiempoCortesDistancia = 12.0f;
}

void FDecoradorCortesDistancia::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	if (NaveContexto->TiempoCortesDistancia > 0.0f)
	{
		UWorld* const World = NaveContexto->GetWorld();
		if (World != nullptr)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = NaveContexto;

			// Disparamos un solo proyectil central simulando el corte de la espada
			FVector SpawnLocation = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, 0.0f, NaveContexto->GunOffset.Z));
			World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocation, FireRotation, SpawnParams);

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("¡CORTE A DISTANCIA!"));
		}
	}
	else
	{
		// Si se acabó el tiempo, el robot vuelve a no poder disparar
		FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
	}
}

FDecoradorRecuperacionRobot::FDecoradorRecuperacionRobot(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	if (Contexto && Contexto->ComponenteCombate) {
		Contexto->ComponenteCombate->VidaActual = Contexto->ComponenteCombate->VidaMaxima;
		Contexto->ComponenteCombate->EscudoActual = Contexto->ComponenteCombate->EscudoMaximo;
	}
}

FDecoradorInmunidad::FDecoradorInmunidad(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	Contexto->TiempoInmunidad = 8.0f;
	Contexto->MultiplicadorDanio = 2.0f;
}