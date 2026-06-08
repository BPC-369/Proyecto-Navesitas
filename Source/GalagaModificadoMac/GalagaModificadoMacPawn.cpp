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
#include "BombaRacimo.h"

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

	ShipMeshComponent->SetCollisionResponseToAllChannels(ECR_Block); // Choca con todo...
	ShipMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); // ...EXCEPTO con la cámara

	// AQUI SE CORRIGE LA ROTACIÓN DE LA NAVE HACIA EL FRENTE
	ShipMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// MALLA STARSPARROW04 MORADA CONFIGURADA AQUÍ
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaNave(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow04.StarSparrow04'"));
	if (FormaNave.Succeeded()) RopaNave = FormaNave.Object;

	RobotMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RobotMesh"));
	RobotMeshComponent->SetupAttachment(GetCapsuleComponent());

	// AQUI ESTÁ LA SOLUCIÓN AL TEMBLOR: Apagamos su colisión física para que no pelee con la cápsula
	RobotMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	RobotMeshComponent->SetCollisionResponseToAllChannels(ECR_Block); // Choca con paredes y balas...
	RobotMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); // ...EXCEPTO con la cámara
	RobotMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	RobotMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // Lo bajamos un poco para que pise el suelo
	RobotMeshComponent->SetVisibility(false); // Nace oculto porque empezamos como Nave

	// 2. LE ASIGNAMOS TU MODELO 3D
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MallaRobotAsset(TEXT("SkeletalMesh'/Game/Geometry/PawnRobot/MallaPawnRobot.MallaPawnRobot'"));
	if (MallaRobotAsset.Succeeded())
	{
		RobotMeshComponent->SetSkeletalMesh(MallaRobotAsset.Object);
	}

	// 3. LE ASIGNAMOS TU ABP_PawnRobot PERFECTO
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRobotAsset(TEXT("AnimBlueprint'/Game/Blueprints/ABP_PawnRobot.ABP_PawnRobot_C'"));
	if (AnimRobotAsset.Succeeded())
	{
		RobotMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		RobotMeshComponent->SetAnimInstanceClass(AnimRobotAsset.Class);
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara3D"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);

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
		ShipMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ShipMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	}

	if (RobotMeshComponent)
	{
		RobotMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RobotMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
		RobotMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -60.0f)); // Forzar posición
	}

	// 2. APLICAR MODELOS
	if (RopaNave != nullptr && ShipMeshComponent != nullptr)
	{
		ShipMeshComponent->SetStaticMesh(RopaNave);
		ShipMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
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
	PlayerInputComponent->BindAction("BotonSaltar", IE_Pressed, this, &AGalagaModificadoMacPawn::EjecutarSalto);
	PlayerInputComponent->BindAction("BotonCorrer", IE_Pressed, this, &AGalagaModificadoMacPawn::IniciarCorrer);
	PlayerInputComponent->BindAction("BotonCorrer", IE_Released, this, &AGalagaModificadoMacPawn::DetenerCorrer);
	PlayerInputComponent->BindAction("AtaqueSecundario", IE_Pressed, this, &AGalagaModificadoMacPawn::AtaqueSecundario);
	PlayerInputComponent->BindAction("DisparoRobot", IE_Pressed, this, &AGalagaModificadoMacPawn::DisparoRobot);
}

void AGalagaModificadoMacPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);
	const float UpValue = GetInputAxisValue(MoveUpBinding);

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

	if (EstadoActual)
	{
		EstadoActual->ActualizarRotacion(this);
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
	if (RobotMeshComponent) RobotMeshComponent->SetVisibility(false);

	if (ShipMeshComponent)
	{
		ShipMeshComponent->SetVisibility(true);
		// Forzamos a que el modelo se mantenga
		if (RopaNave) ShipMeshComponent->SetStaticMesh(RopaNave);
	}

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}

	// --- MAGIA DE CÁMARA: MODO NAVE ---
	if (CameraBoom)
	{
		// FALSE: La cámara se "pega" a la nave y gira junto con ella.
		// Esto te permite hacer maniobras espaciales libremente.
		CameraBoom->SetUsingAbsoluteRotation(false);

		// Restauramos su ángulo original para volar
		CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	}
}

void AGalagaModificadoMacPawn::ConvertirEnRobot()
{
	if (ShipMeshComponent) ShipMeshComponent->SetVisibility(false);
	if (RobotMeshComponent) RobotMeshComponent->SetVisibility(true);

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	// --- MAGIA DE CÁMARA: MODO ROBOT ---
	if (CameraBoom)
	{
		// TRUE: La cámara ignora los giros del robot. 
		// Esto evita el "Tornado" porque el robot puede girar hacia el cursor del mouse 
		// sin arrastrar a la cámara con él.
		CameraBoom->SetUsingAbsoluteRotation(true);

		// Acomodamos la cámara para darte una buena vista táctica "Top-Down / Isométrica"
		// Le damos un ángulo un poco más picado hacia abajo (-30 grados) para ver bien a los enemigos
		CameraBoom->SetWorldRotation(FRotator(-30.f, 0.f, 0.f));
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

void AGalagaModificadoMacPawn::EjecutarSalto() { 
	if (EstadoActual) EstadoActual->ManejarSalto(this); 
}

void AGalagaModificadoMacPawn::AtaqueSecundario() { 
	if (EstadoActual) EstadoActual->EjecutarAtaqueSecundario(this); 

}

void AGalagaModificadoMacPawn::DisparoRobot() { 
	if (EstadoActual) EstadoActual->EjecutarDisparoRobot(this); 
}

// Para correr (Afecta a ambos modos)
void AGalagaModificadoMacPawn::IniciarCorrer() { 
	MoveSpeed = 800.0f; GetCharacterMovement()->MaxWalkSpeed = 800.0f; 
}

void AGalagaModificadoMacPawn::DetenerCorrer() { 
	MoveSpeed = 300.0f; GetCharacterMovement()->MaxWalkSpeed = 300.0f; 
}

// PATRÓN STATE
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

void FEstadoNaveVoladora::EjecutarAtaqueSecundario(AGalagaModificadoMacPawn* NaveContexto) {}
void FEstadoNaveVoladora::EjecutarDisparoRobot(AGalagaModificadoMacPawn* NaveContexto) {}
void FEstadoNaveVoladora::ManejarSalto(AGalagaModificadoMacPawn* NaveContexto) {}
void FEstadoNaveVoladora::ActualizarRotacion(AGalagaModificadoMacPawn* NaveContexto) {
	// La nave usa los ejes del mouse para hacer "Barrel Rolls" o girar como avión
	const float ValorMouseX = NaveContexto->GetInputAxisValue("Turn");
	const float ValorMouseY = NaveContexto->GetInputAxisValue("LookUp");

	if (ValorMouseX != 0.0f || ValorMouseY != 0.0f)
	{
		FRotator RotacionNave = FRotator(ValorMouseY * 3.0f, ValorMouseX * 3.0f, 0.0f);
		NaveContexto->AddActorLocalRotation(RotacionNave);
	}
}

// 1. ROTACIÓN HACIA EL MOUSE
void FEstadoNaveRobot::ActualizarRotacion(AGalagaModificadoMacPawn* NaveContexto)
{
	// A. GIRAR SOLO LA CÁMARA (Independiente del robot)
	const float ValorMouseX = NaveContexto->GetInputAxisValue("Turn");
	const float ValorMouseY = NaveContexto->GetInputAxisValue("LookUp");

	if (NaveContexto->GetCameraBoom() != nullptr && (ValorMouseX != 0.0f || ValorMouseY != 0.0f))
	{
		// Giramos el "brazo" de la cámara. Puedes cambiar el 1.5f para que la cámara gire más rápido o más lento.
		FRotator RotacionCamara = FRotator(ValorMouseY * 1.5f, ValorMouseX * 1.5f, 0.0f);
		NaveContexto->GetCameraBoom()->AddLocalRotation(RotacionCamara);
	}

	// B. APUNTAR EL CUERPO DEL ROBOT AL MOUSE
	APlayerController* PC = Cast<APlayerController>(NaveContexto->GetController());
	if (PC)
	{
		FHitResult HitResult;
		// Lanza un rayo desde el mouse al mundo 3D
		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			FVector DireccionMouse = HitResult.ImpactPoint - NaveContexto->GetActorLocation();
			DireccionMouse.Z = 0.0f; // Evitamos que el robot mire hacia el cielo o se entierre
			NaveContexto->SetActorRotation(DireccionMouse.Rotation());
		}
	}
}

// 2. ATAQUE NORMAL / RARO (Probabilidad)
void FEstadoNaveRobot::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
	// Probabilidad del 20% de hacer el ataque raro
	bool bEsAtaqueRaro = FMath::RandRange(1, 100) <= 20;

	if (bEsAtaqueRaro && NaveContexto->MontajeAtaqueRaro)
	{
		NaveContexto->GetRobotMeshComponent()->GetAnimInstance()->Montage_Play(NaveContexto->MontajeAtaqueRaro);
	}
	else if (NaveContexto->MontajeAtaqueNormal)
	{
		NaveContexto->GetRobotMeshComponent()->GetAnimInstance()->Montage_Play(NaveContexto->MontajeAtaqueNormal);
	}

	// (Aquí mantienes tu código anterior de detectar colisiones con OverlapMultiByChannel para el daño)
}

// 3. ATAQUE CARGADO (Click Derecho)
void FEstadoNaveRobot::EjecutarAtaqueSecundario(AGalagaModificadoMacPawn* NaveContexto)
{
	if (NaveContexto->MontajeAtaqueCargado)
	{
		NaveContexto->GetRobotMeshComponent()->GetAnimInstance()->Montage_Play(NaveContexto->MontajeAtaqueCargado);
		// Lógica de daño expansivo o mayor daño aquí
	}
}

// 4. DISPARO DEL ROBOT (Tecla G)
void FEstadoNaveRobot::EjecutarDisparoRobot(AGalagaModificadoMacPawn* NaveContexto)
{
	if (NaveContexto->MontajeDisparoRobot)
	{
		NaveContexto->GetRobotMeshComponent()->GetAnimInstance()->Montage_Play(NaveContexto->MontajeDisparoRobot);
	}

	// Disparamos el proyectil
	UWorld* const World = NaveContexto->GetWorld();
	if (World != nullptr)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = NaveContexto;
		World->SpawnActor<AGalagaModificadoMacProjectile>(NaveContexto->GetActorLocation() + (NaveContexto->GetActorForwardVector() * 100.f), NaveContexto->GetActorRotation(), SpawnParams);
	}
}

// 5. SALTO Y DASH (Doble Espacio)
void FEstadoNaveRobot::ManejarSalto(AGalagaModificadoMacPawn* NaveContexto)
{
	float TiempoActual = NaveContexto->GetWorld()->GetTimeSeconds();

	// Si pulsó espacio hace menos de 0.4 segundos -> DASH
	if (TiempoActual - NaveContexto->TiempoUltimoSalto <= 0.4f)
	{
		if (NaveContexto->MontajeDash)
		{
			NaveContexto->GetRobotMeshComponent()->GetAnimInstance()->Montage_Play(NaveContexto->MontajeDash);
		}
		// Lo empujamos hacia adelante a gran velocidad
		NaveContexto->LaunchCharacter(NaveContexto->GetActorForwardVector() * 2000.0f, true, true);
	}
	else
	{
		// Salto Normal
		NaveContexto->Jump();
		NaveContexto->TiempoUltimoSalto = TiempoActual;
	}
}

// 6. TRANSFORMACIÓN (Apagar malla de nave, encender robot)
void FEstadoNaveRobot::EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto)
{
	NaveContexto->GetShipMeshComponent()->SetVisibility(true);
	NaveContexto->GetRobotMeshComponent()->SetVisibility(false);
	NaveContexto->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

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

