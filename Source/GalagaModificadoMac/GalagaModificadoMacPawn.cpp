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
#include "Blueprint/UserWidget.h"			
#include "GameFramework/PlayerController.h"	

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

	// AQUI SE CORRIGE LA ROTACIÓN DE LA NAVE HACIA EL FRENTE
	ShipMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// MALLA STARSPARROW04 MORADA CONFIGURADA AQUÍ
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaNave(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow04.StarSparrow04'"));
	if (FormaNave.Succeeded()) RopaNave = FormaNave.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCubo(TEXT("StaticMesh'/Game/Geometry/pawn/pawn09.pawn09'"));
	if (FormaCubo.Succeeded()) RopaCubo = FormaCubo.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara3D"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(false);

	// AQUI SE ALEJA LA CÁMARA (1200.f en lugar de 250.f)
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);
	CameraBoom->bDoCollisionTest = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TerceraPersonaCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	CameraBoom->bUsePawnControlRotation = false;

	ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));

	MoveSpeed = 1500.0f;
	VelocidadOriginalNave = 1500.0f;

	GunOffset = FVector(130.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	bEstaDisparando = false;

	MultiplicadorDanio = 1.0f;
	TiempoDisparoCuadruple = 0.0f;
	BombasRacimoRestantes = 0;
	TiempoBuffoNave = 0.0f;
	TiempoBuffoRobot = 0.0f;
	TiempoCortesDistancia = 0.0f;
	TiempoInmunidad = 0.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	EstadoActual = new FEstadoNaveVoladora();

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 2000.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Jugador");
	}

	bMuerto = false;
	WidgetGameOverClass = nullptr;
}

void AGalagaModificadoMacPawn::BeginPlay()
{
	Super::BeginPlay();
	if (ShipMeshComponent)
	{
		ShipMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		// Nota: Si con -90.f la nave mira hacia ti en lugar de hacia adelante, cámbialo a 90.f positivo.
	}
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

void FEstadoNaveVoladora::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	UWorld* const World = NaveContexto->GetWorld();
	if (World != nullptr)
	{
		const FRotator FireRotation = FireDirection.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = NaveContexto;

		if (NaveContexto->BombasRacimoRestantes > 0)
		{
			NaveContexto->BombasRacimoRestantes--;
			for (int i = 0; i < 8; i++)
			{
				FRotator RacimoRot = FireRotation;
				RacimoRot.Yaw += (45.0f * i);
				World->SpawnActor<AGalagaModificadoMacProjectile>(NaveContexto->GetActorLocation(), RacimoRot, SpawnParams);
			}
		}
		else if (NaveContexto->TiempoDisparoCuadruple > 0.0f)
		{
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
		}
		else
		{
			float SeparacionCanones = 23.0f;
			FVector OffsetIzquierdo = FVector(NaveContexto->GunOffset.X, -SeparacionCanones, NaveContexto->GunOffset.Z);
			FVector OffsetDerecho = FVector(NaveContexto->GunOffset.X, SeparacionCanones, NaveContexto->GunOffset.Z);

			FVector SpawnLocationIzquierdo = NaveContexto->GetActorLocation() + FireRotation.RotateVector(OffsetIzquierdo);
			FVector SpawnLocationDerecho = NaveContexto->GetActorLocation() + FireRotation.RotateVector(OffsetDerecho);

			World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocationIzquierdo, FireRotation, SpawnParams);
			World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocationDerecho, FireRotation, SpawnParams);
		}
	}
}

void FEstadoNaveRobot::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("¡Los robots no pueden disparar!"));
}

float AGalagaModificadoMacPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (TiempoInmunidad > 0.0f) return 0.0f;

	float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ComponenteCombate != nullptr)
	{
		DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);

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
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	OnDeathEvent();

	if (WidgetGameOverClass)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetGameOverClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}
	}
}

float AGalagaModificadoMacPawn::GetVidaActual() const
{
	if (ComponenteCombate)
		return ComponenteCombate->VidaActual;
	return 0.0f;
}

float AGalagaModificadoMacPawn::GetVidaMaxima() const
{
	if (ComponenteCombate)
		return ComponenteCombate->VidaMaxima;
	return 1.0f;
}

// PATRÓN DECORATOR
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
	FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
}

FDecoradorBombasRacimo::FDecoradorBombasRacimo(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
	Contexto->BombasRacimoRestantes += 6;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("BOMBITAS"));
}

void FDecoradorBombasRacimo::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	if (NaveContexto->BombasRacimoRestantes > 0)
	{
		UWorld* const World = NaveContexto->GetWorld();
		if (World != nullptr)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = NaveContexto;

			NaveContexto->BombasRacimoRestantes--;

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

			FVector SpawnLocation = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, 0.0f, NaveContexto->GunOffset.Z));
			World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocation, FireRotation, SpawnParams);

			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, TEXT("¡CORTE A DISTANCIA!"));
		}
	}
	else
	{
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

// IMPLEMENTACIONES PARA ESTADOS DE TRANSFORMACIÓN
void FEstadoNaveVoladora::EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto)
{
	if (NaveContexto != nullptr)
	{
		NaveContexto->ConvertirEnRobot();
		NaveContexto->CambiarEstado(new FEstadoNaveRobot());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("¡Transformación: Modo Robot Activo!"));
	}
}

void FEstadoNaveRobot::EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto)
{
	if (NaveContexto != nullptr)
	{
		NaveContexto->ConvertirEnNave();
		NaveContexto->CambiarEstado(new FEstadoNaveVoladora());
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("¡Transformación: Modo Nave Activo!"));
	}
}

// IMPLEMENTACIÓN PARA DETECTAR COLISIONES (OVERLAP)
void AGalagaModificadoMacPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}