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
#include "BombaRacimo.h"
#include "GalagaModificadoMacGameMode.h"
#include "GalagaGameInstance.h"

// HUD
#include "BossEstatico.h"
#include "NaveComando.h"
#include "CeldasEnergia.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/PanelWidget.h"
#include "EngineUtils.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/PrimitiveComponent.h"

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
    ShipMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaNave(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow04.StarSparrow04'"));
    if (FormaNave.Succeeded()) RopaNave = FormaNave.Object;

    RobotMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RobotMesh"));
    RobotMeshComponent->SetupAttachment(GetCapsuleComponent());
    RobotMeshComponent->SetCollisionProfileName(TEXT("NoCollision")); // Fantasma físico
    RobotMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    RobotMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -60.0f)); // Alineamos los pies al piso
    RobotMeshComponent->SetVisibility(false); // Nace oculto

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MallaRobotAsset(TEXT("SkeletalMesh'/Game/Geometry/PawnRobot/MallaPawnRobot.MallaPawnRobot'"));
    if (MallaRobotAsset.Succeeded()) RobotMeshComponent->SetSkeletalMesh(MallaRobotAsset.Object);

    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimRobotAsset(TEXT("AnimBlueprint'/Game/Blueprints/ABP_PawnRobot.ABP_PawnRobot_C'"));
    if (AnimRobotAsset.Succeeded())
    {
        RobotMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        RobotMeshComponent->SetAnimInstanceClass(AnimRobotAsset.Class);
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("SoundWave'/Game/music/lasecito.lasecito'"));
    FireSound = FireAudio.Object;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("BrazoCamara3D"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetUsingAbsoluteRotation(false);
    CameraBoom->TargetArmLength = 1200.f;
    CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
    CameraBoom->SocketOffset = FVector(0.f, 0.f, 50.f);
    CameraBoom->bDoCollisionTest = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TerceraPersonaCamera"));
    CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;
    CameraBoom->bUsePawnControlRotation = false;

    ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));

    MoveSpeed = 2500.0f;
    VelocidadOriginalNave = 2500.0f;

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

    EstadoActual = nullptr;

    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 2000.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->Faccion = FName("Jugador");
    }

    bMuerto = false;
    WidgetGameOverClass = nullptr;

    // Carga de widgets para las barras de vida, crosshair y pausa
    static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarBP(TEXT("/Game/Blueprints/WBP_EnemyHealthBar"));
    if (HealthBarBP.Succeeded()) EnemyHealthBarClass = HealthBarBP.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> EnemyListBP(TEXT("/Game/Blueprints/WBP_EnemyList"));
    if (EnemyListBP.Succeeded()) EnemyListClass = EnemyListBP.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> BossBarBP(TEXT("/Game/Blueprints/WBP_BossHealth"));
    if (BossBarBP.Succeeded()) BossHealthClass = BossBarBP.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> CrosshairBP(TEXT("/Game/Blueprints/WBP_Crosshair"));
    if (CrosshairBP.Succeeded()) CrosshairClass = CrosshairBP.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuBP(TEXT("/Game/Blueprints/WBP_PauseMenu"));
    if (PauseMenuBP.Succeeded()) PauseMenuClass = PauseMenuBP.Class;
}

void AGalagaModificadoMacPawn::BeginPlay()
{
    Super::BeginPlay();

    EstadoActual = new FEstadoNaveVoladora();

    if (ShipMeshComponent)
        ShipMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    ConvertirEnNave();

    if (EnemyListClass)
    {
        EnemyListWidget = CreateWidget<UUserWidget>(GetWorld(), EnemyListClass);
        if (EnemyListWidget) EnemyListWidget->AddToViewport(10);
    }

    if (BossHealthClass)
    {
        BossHealthWidget = CreateWidget<UUserWidget>(GetWorld(), BossHealthClass);
        if (BossHealthWidget) BossHealthWidget->AddToViewport();
    }

    if (CrosshairClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairClass);
        if (CrosshairWidget) CrosshairWidget->AddToViewport(100);
    }
}

void AGalagaModificadoMacPawn::Destroyed()
{
    Super::Destroyed();

    // Destruimos el estado actual de la memoria cuando el Pawn se destruye
    if (EstadoActual != nullptr)
    {
        delete EstadoActual;
        EstadoActual = nullptr;
    }
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
    PlayerInputComponent->BindAxis("LookUp", this, &AGalagaModificadoMacPawn::InvertirMouseFPS);
    PlayerInputComponent->BindAction("Disparar", IE_Pressed, this, &AGalagaModificadoMacPawn::EmpezarDisparo);
    PlayerInputComponent->BindAction("Disparar", IE_Released, this, &AGalagaModificadoMacPawn::DetenerDisparo);
    PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AGalagaModificadoMacPawn::OnPauseButtonPressed);
    
    PlayerInputComponent->BindAction("AtaqueCargado", IE_Pressed, this, &AGalagaModificadoMacPawn::AtaqueSecundario);
    PlayerInputComponent->BindAction("Correr", IE_Pressed, this, &AGalagaModificadoMacPawn::IniciarCorrer);
    PlayerInputComponent->BindAction("Correr", IE_Released, this, &AGalagaModificadoMacPawn::DetenerCorrer);
    PlayerInputComponent->BindAction("DisparoQ", IE_Pressed, this, &AGalagaModificadoMacPawn::DisparoEspecial);
    PlayerInputComponent->BindAction("Saltar", IE_Pressed, this, &AGalagaModificadoMacPawn::EjecutarSalto);
}

void AGalagaModificadoMacPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (bIsPaused) return;

    const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
    const float RightValue = GetInputAxisValue(MoveRightBinding);
    const float UpValue = GetInputAxisValue(MoveUpBinding);

    if (GetCharacterMovement() && GetCharacterMovement()->MovementMode == MOVE_Flying)
    {
        const float ValorMouseX = GetInputAxisValue("Turn");
        const float ValorMouseY = GetInputAxisValue("LookUp");

        if (CameraBoom != nullptr && (ValorMouseX != 0.0f || ValorMouseY != 0.0f))
        {
            // 1. Obtenemos la rotación actual
            FRotator RotacionActual = GetActorRotation();

            // 2. Calculamos la nueva dirección
            float NuevoPitch = RotacionActual.Pitch + (ValorMouseY * 3.0f);
            float NuevoYaw = RotacionActual.Yaw + (ValorMouseX * 3.0f);

            // 3. LIMITAMOS que tanto puede subir o bajar la nariz (Ej: -60 a 60 grados)
            // Esto evita que la nave de una vuelta completa hacia atrás y quede de cabeza
            NuevoPitch = FMath::Clamp(NuevoPitch, -60.0f, 60.0f);

            // 4. APLICAMOS, forzando el Roll a 0.0f para que SIEMPRE esté "de pie"
            FRotator NuevaRotacion = FRotator(NuevoPitch, NuevoYaw, 0.0f);

            SetActorRotation(NuevaRotacion);
        }
    }
    else if (GetCharacterMovement() && GetCharacterMovement()->MovementMode == MOVE_Walking)
    {
        // CONTROLES DEL ROBOT (El cuerpo apunta directamente al cursor)
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            FHitResult HitResult;
            if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
            {
                FVector DireccionMouse = HitResult.ImpactPoint - GetActorLocation();
                DireccionMouse.Z = 0.0f; // Evita que el robot se incline hacia arriba o abajo
                SetActorRotation(DireccionMouse.Rotation());
            }
        }
    }

    AddMovementInput(GetActorForwardVector(), ForwardValue);
    AddMovementInput(GetActorRightVector(), RightValue);
    AddMovementInput(GetActorUpVector(), UpValue);

    if (bEstaDisparando && bCanFire) FireShot(GetActorForwardVector());

    if (TiempoDisparoCuadruple > 0.0f) TiempoDisparoCuadruple -= DeltaSeconds;
    if (TiempoCortesDistancia > 0.0f) TiempoCortesDistancia -= DeltaSeconds;

    if (TiempoInmunidad > 0.0f) { TiempoInmunidad -= DeltaSeconds; if (TiempoInmunidad <= 0.0f) MultiplicadorDanio = 1.0f; }
    if (TiempoBuffoNave > 0.0f) { TiempoBuffoNave -= DeltaSeconds; if (TiempoBuffoNave <= 0.0f) { MoveSpeed = VelocidadOriginalNave; GetCharacterMovement()->MaxFlySpeed = MoveSpeed; MultiplicadorDanio = 1.0f; } }
    if (TiempoBuffoRobot > 0.0f) { TiempoBuffoRobot -= DeltaSeconds; if (TiempoBuffoRobot <= 0.0f) { MoveSpeed = 300.0f; GetCharacterMovement()->MaxWalkSpeed = MoveSpeed; } }

    UpdateHealthBars(DeltaSeconds);

    // Dynamic crosshair
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && CrosshairWidget)
    {
        FVector CamLoc; FRotator CamRot;
        PC->GetPlayerViewPoint(CamLoc, CamRot);
        FVector Start = CamLoc;
        FVector End = Start + (CamRot.Vector() * 5000.0f);
        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
        bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);
        bool bFound = false;
        if (bHit && Hit.GetActor())
        {
            UComponenteCombate* Comp = Hit.GetActor()->FindComponentByClass<UComponenteCombate>();
            if (Comp && Comp->Faccion == FName("Enemigo")) bFound = true;
        }
        if (bFound != bEnemyTargeted)
        {
            bEnemyTargeted = bFound;
            UFunction* Func = CrosshairWidget->FindFunction(FName("SetTargeted"));
            if (Func)
            {
                struct FParams { bool bTargeted; };
                FParams ParamsStruct;
                ParamsStruct.bTargeted = bFound;
                CrosshairWidget->ProcessEvent(Func, &ParamsStruct);
            }
        }
    }
}

void AGalagaModificadoMacPawn::FireShot(FVector FireDirection)
{
    if (EstadoActual != nullptr) EstadoActual->EjecutarAtaque(this, FireDirection);
    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AGalagaModificadoMacPawn::ShotTimerExpired, FireRate);
    if (FireSound != nullptr) UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void AGalagaModificadoMacPawn::ShotTimerExpired() { bCanFire = true; }

void AGalagaModificadoMacPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (GetCharacterMovement() && GetCharacterMovement()->MovementMode == MOVE_Walking)
    {
        if (OtherActor != nullptr && OtherActor != this)
        {
            UComponenteCombate* CompEnemigo = OtherActor->FindComponentByClass<UComponenteCombate>();
            if (CompEnemigo != nullptr && CompEnemigo->Faccion == FName("Enemigo"))
            {
                float DanioPorChoque = 15.0f;
                UGameplayStatics::ApplyDamage(OtherActor, DanioPorChoque * MultiplicadorDanio, GetController(), this, UDamageType::StaticClass());
                GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("¡EMBESTIDA ROBOT!"));
            }
        }
    }
}

void AGalagaModificadoMacPawn::InvertirMouseFPS(float Valor)
{
    // Multiplicamos por -1 para invertir el eje y lograr un control FPS tradicional.
    // Esto solo afectará al Robot, porque la Nave procesa su propia rotación en el Tick.
    AddControllerPitchInput(Valor * -1.0f);
}

void AGalagaModificadoMacPawn::EmpezarDisparo() { bEstaDisparando = true; }
void AGalagaModificadoMacPawn::DetenerDisparo() { bEstaDisparando = false; }

void AGalagaModificadoMacPawn::CambiarEstado(IEstadoNave* NuevoEstado)
{
    if (EstadoActual != nullptr) delete EstadoActual;
    EstadoActual = NuevoEstado;
}

void AGalagaModificadoMacPawn::ConvertirEnNave()
{
    // 1. Ocultar Robot, Mostrar Nave
    if (RobotMeshComponent) RobotMeshComponent->SetVisibility(false);
    if (ShipMeshComponent)
    {
        ShipMeshComponent->SetVisibility(true);
        if (RopaNave) ShipMeshComponent->SetStaticMesh(RopaNave);
    }

    if (GetCharacterMovement()) GetCharacterMovement()->SetMovementMode(MOVE_Flying);

    // 3. RESTAURAR CÁMARA ORIGINAL DE LA NAVE
    if (CameraBoom)
    {
        CameraBoom->TargetArmLength = 800.f; // <-- DE VUELTA A TU VALOR ORIGINAL
        CameraBoom->SocketOffset = FVector(0.f, 0.f, 150.f);
        CameraBoom->bUsePawnControlRotation = false;
        CameraBoom->SetUsingAbsoluteRotation(false);
        CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
    }

    bUseControllerRotationYaw = false;
}

void AGalagaModificadoMacPawn::ConvertirEnRobot()
{
    // 1. Ocultar Nave, Mostrar Robot
    if (ShipMeshComponent) ShipMeshComponent->SetVisibility(false);
    if (RobotMeshComponent) RobotMeshComponent->SetVisibility(true);

    // 2. Físicas de caminar
    if (GetCharacterMovement()) GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // 3. CÁMARA DE ROBOT (Primera Persona)
    if (CameraBoom)
    {
        CameraBoom->TargetArmLength = 400.f; // Pegamos la cámara completamente al cuerpo (0 de distancia)

        CameraBoom->SocketOffset = FVector::ZeroVector;
        // Movemos la cámara a la altura de la cabeza (Z=60) 
        // y la empujamos un poco hacia adelante (X=40) para que no veas la espalda del robot por dentro
        CameraBoom->SocketOffset = FVector(20.f, 0.f, 100.f);
        CameraBoom->SetUsingAbsoluteRotation(false);

        // MAGIA: Le decimos a la cámara que obedezca directamente al movimiento del mouse
        CameraBoom->bUsePawnControlRotation = true;
    }

    // MAGIA 2: Le decimos al cuerpo del robot que gire de izquierda a derecha cuando muevas la cámara
    bUseControllerRotationYaw = true;
}

void AGalagaModificadoMacPawn::Transformar()
{
    if (EstadoActual != nullptr) EstadoActual->EjecutarTransformacion(this);
}

float AGalagaModificadoMacPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (TiempoInmunidad > 0.0f) return 0.0f;
    float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ComponenteCombate != nullptr)
    {
        DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);
        if (!bMuerto && ComponenteCombate->VidaActual <= 0.0f) { bMuerto = true; ManejarMuerte(); }
    }
    return DanioReal;
}

void AGalagaModificadoMacPawn::ManejarMuerte()
{
    for (auto& Par : EnemyHealthWidgets) { if (Par.Value) Par.Value->RemoveFromParent(); }
    EnemyHealthWidgets.Empty();
    if (EnemyListWidget) { EnemyListWidget->RemoveFromParent(); EnemyListWidget = nullptr; }
    if (BossHealthWidget) { BossHealthWidget->RemoveFromParent(); BossHealthWidget = nullptr; }
    if (CrosshairWidget) { CrosshairWidget->RemoveFromParent(); CrosshairWidget = nullptr; }
    if (PauseMenuWidget) { PauseMenuWidget->RemoveFromParent(); PauseMenuWidget = nullptr; }

    AGalagaModificadoMacGameMode* GM = Cast<AGalagaModificadoMacGameMode>(GetWorld()->GetAuthGameMode());
    if (GM) GM->DetenerMusica();

    UGameplayStatics::SetGamePaused(GetWorld(), true);

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC) { FInputModeUIOnly InputMode; InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); PC->SetInputMode(InputMode); PC->bShowMouseCursor = true; }

    OnDeathEvent();
    if (WidgetGameOverClass) { UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetGameOverClass); if (GameOverWidget) GameOverWidget->AddToViewport(); }
}

float AGalagaModificadoMacPawn::GetVidaActual() const { return ComponenteCombate ? ComponenteCombate->VidaActual : 0.0f; }
float AGalagaModificadoMacPawn::GetVidaMaxima() const { return ComponenteCombate ? ComponenteCombate->VidaMaxima : 1.0f; }

// ========== BARRAS DE VIDA EN HUD ==========
void AGalagaModificadoMacPawn::UpdateHealthBars(float DeltaSeconds)
{
    TArray<AActor*> EnemigosActuales;
    for (TActorIterator<ANaveComando> It(GetWorld()); It; ++It) if (*It && !(*It)->IsPendingKill()) EnemigosActuales.Add(*It);
    for (TActorIterator<ACeldaEnergia> It(GetWorld()); It; ++It) if (*It && !(*It)->IsPendingKill()) EnemigosActuales.Add(*It);

    TArray<AActor*> ParaEliminar;
    for (auto& Par : EnemyHealthWidgets) if (!Par.Key || Par.Key->IsPendingKill() || !EnemigosActuales.Contains(Par.Key)) { if (Par.Value) Par.Value->RemoveFromParent(); ParaEliminar.Add(Par.Key); }
    for (AActor* Key : ParaEliminar) EnemyHealthWidgets.Remove(Key);

    for (AActor* Enemy : EnemigosActuales) if (!EnemyHealthWidgets.Contains(Enemy)) { UUserWidget* Widget = CreateHealthBarForEnemy(Enemy); EnemyHealthWidgets.Add(Enemy, Widget); }

    UPanelWidget* ListaPanel = nullptr;
    if (EnemyListWidget) ListaPanel = Cast<UPanelWidget>(EnemyListWidget->GetWidgetFromName(TEXT("EnemyListPanel")));

    if (ListaPanel)
    {
        EnemigosActuales.Sort([](const AActor& A, const AActor& B) {
            if (A.IsA(ANaveComando::StaticClass()) && !B.IsA(ANaveComando::StaticClass())) return true;
            return false;
            });
        ListaPanel->ClearChildren();
        for (AActor* Enemy : EnemigosActuales)
        {
            UUserWidget* Widget = EnemyHealthWidgets.Contains(Enemy) ? EnemyHealthWidgets[Enemy] : nullptr;
            if (Widget)
            {
                ListaPanel->AddChild(Widget);
                if (UVerticalBoxSlot* Slot = Cast<UVerticalBoxSlot>(Widget->Slot)) Slot->SetPadding(FMargin(0, 0, 0, 10));
                float Percent = 1.0f;
                if (ANaveComando* N = Cast<ANaveComando>(Enemy)) { UComponenteCombate* Comp = N->GetComponenteCombate(); if (Comp) Percent = Comp->VidaActual / Comp->VidaMaxima; }
                else if (ACeldaEnergia* C = Cast<ACeldaEnergia>(Enemy)) Percent = C->GetVida() / C->GetVidaMaxima();
                UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("ProgressBar_0")));
                if (Bar) { float Cur = Bar->Percent; float NewP = FMath::FInterpTo(Cur, Percent, DeltaSeconds, 6.0f); Bar->SetPercent(NewP); }
            }
        }
    }

    if (BossHealthWidget)
    {
        ABossEstatico* Boss = nullptr;
        for (TActorIterator<ABossEstatico> It(GetWorld()); It; ++It) { Boss = *It; break; }
        UProgressBar* Bar = Cast<UProgressBar>(BossHealthWidget->GetWidgetFromName(TEXT("ProgressBar_0")));
        if (Bar)
        {
            if (Boss && !Boss->IsPendingKill())
            {
                float Percent = (Boss->VidaMaxima > 0.0f) ? (Boss->VidaJefe / Boss->VidaMaxima) : 0.0f;
                float Cur = Bar->Percent; float NewP = FMath::FInterpTo(Cur, Percent, DeltaSeconds, 6.0f);
                Bar->SetPercent(NewP); Bar->SetVisibility(ESlateVisibility::Visible);
            }
            else Bar->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

UUserWidget* AGalagaModificadoMacPawn::CreateHealthBarForEnemy(AActor* Enemy)
{
    if (!EnemyHealthBarClass || !EnemyListWidget) return nullptr;
    return CreateWidget<UUserWidget>(GetWorld(), EnemyHealthBarClass);
}
// ===========================================

// ========== PAUSA ==========
void AGalagaModificadoMacPawn::OnPauseButtonPressed()
{
    if (bMuerto) return;
    if (bIsPaused) { ResumeGame(); return; }

    bIsPaused = true;
    UGameplayStatics::SetGamePaused(GetWorld(), true);

    if (PauseMenuClass && !PauseMenuWidget)
    {
        PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);
        if (PauseMenuWidget)
        {
            PauseMenuWidget->AddToViewport(200);
            APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PC)
            {
                FInputModeGameAndUI InputMode;
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
        }
    }
}

void AGalagaModificadoMacPawn::ResumeGame()
{
    if (!bIsPaused) return;
    bIsPaused = false;
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    if (PauseMenuWidget) { PauseMenuWidget->RemoveFromParent(); PauseMenuWidget = nullptr; }
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC) { PC->SetInputMode(FInputModeGameOnly()); PC->bShowMouseCursor = false; }
}

void AGalagaModificadoMacPawn::ReturnToMainMenuFromPause()
{
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    if (UGalagaGameInstance* GI = Cast<UGalagaGameInstance>(GetGameInstance())) { GI->ReturnToMainMenu(); return; }
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenuLevel"));
}

void AGalagaModificadoMacPawn::PauseResumeGame() { ResumeGame(); }

void AGalagaModificadoMacPawn::PauseReturnToMainMenu() { ReturnToMainMenuFromPause(); }

// --- LÓGICA DE MOVIMIENTO ---
void AGalagaModificadoMacPawn::IniciarCorrer()
{
    if (GetCharacterMovement()->MovementMode == MOVE_Walking) {
        MoveSpeed = 600.0f; // Duplicamos velocidad base
        GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    }
}

void AGalagaModificadoMacPawn::DetenerCorrer()
{
    if (GetCharacterMovement()->MovementMode == MOVE_Walking) {
        MoveSpeed = 300.0f; // Volvemos a caminar normal
        GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
    }
}

void AGalagaModificadoMacPawn::EjecutarSalto()
{
    if (GetCharacterMovement()->MovementMode == MOVE_Walking) {
        Jump(); // ACharacter ya tiene la lógica de físicas para saltar
    }
}

// --- LÓGICA DE ATAQUES EXTRA ---
void AGalagaModificadoMacPawn::AtaqueSecundario()
{
    if (GetCharacterMovement()->MovementMode == MOVE_Walking && RobotMeshComponent && MontajeAtaqueCargado)
    {
        // 1. Reproducir la animación
        RobotMeshComponent->GetAnimInstance()->Montage_Play(MontajeAtaqueCargado);

        // 2. Lógica de Daño en Área (Una gran explosión circular alrededor del robot)
        UWorld* const World = GetWorld();
        if (World != nullptr)
        {
            FVector CentroExplosion = GetActorLocation(); // El centro eres tú
            float RadioExplosion = 600.0f; // Un área mucho más grande
            float DanioCargado = 75.0f; // Triple daño que el ataque normal

            TArray<FOverlapResult> EnemigosGolpeados;
            FCollisionQueryParams ParametrosColision;
            ParametrosColision.AddIgnoredActor(this);

            bool bHuboGolpe = World->OverlapMultiByChannel(
                EnemigosGolpeados, CentroExplosion, FQuat::Identity,
                ECollisionChannel::ECC_Pawn, FCollisionShape::MakeSphere(RadioExplosion), ParametrosColision
            );

            if (bHuboGolpe)
            {
                for (FOverlapResult& Overlap : EnemigosGolpeados)
                {
                    AActor* ActorGolpeado = Overlap.GetActor();
                    if (ActorGolpeado != nullptr)
                    {
                        UComponenteCombate* CompEnemigo = ActorGolpeado->FindComponentByClass<UComponenteCombate>();
                        if (CompEnemigo != nullptr && CompEnemigo->Faccion == FName("Enemigo"))
                        {
                            UGameplayStatics::ApplyDamage(ActorGolpeado, DanioCargado * MultiplicadorDanio, GetController(), this, UDamageType::StaticClass());
                        }
                    }
                }
            }
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("¡EXPLOSIÓN CARGADA!"));
        }
    }
}

void AGalagaModificadoMacPawn::DisparoEspecial()
{
    if (GetCharacterMovement()->MovementMode == MOVE_Walking && RobotMeshComponent && MontajeDisparoQ)
    {
        // 1. Reproducir la animación
        RobotMeshComponent->GetAnimInstance()->Montage_Play(MontajeDisparoQ);

        // 2. Lógica de Disparo (Igual que la nave)
        UWorld* const World = GetWorld();
        if (World != nullptr)
        {
            // Disparamos hacia donde está mirando el robot
            FRotator FireRotation = GetActorRotation();

            // Empujamos el proyectil un poco hacia adelante (100 unidades) para que no nazca dentro de ti
            FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(FVector(100.f, 0.f, 0.f));

            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;

            // Generar el proyectil
            World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocation, FireRotation, SpawnParams);

            if (FireSound != nullptr) {
                UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
            }
        }
    }
}
// ===========================

// --- PATRÓN STATE ---
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

void FEstadoNaveRobot::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
    // =========================================================================
    // PARTE 1: LAS ANIMACIONES (El 20% de probabilidad)
    // =========================================================================
    if (NaveContexto->GetRobotMeshComponent() && NaveContexto->GetRobotMeshComponent()->GetAnimInstance())
    {
        // Tiramos los dados: del 1 al 100. Si cae 20 o menos, es ataque raro.
        bool bEsAtaqueRaro = FMath::RandRange(1, 100) <= 20;

        if (bEsAtaqueRaro && NaveContexto->MontajeAtaqueRaro)
        {
            NaveContexto->GetRobotMeshComponent()->GetAnimInstance()->Montage_Play(NaveContexto->MontajeAtaqueRaro);
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Purple, TEXT("¡ATAQUE RARO ACTIVADO!"));
        }
        else if (NaveContexto->MontajeAtaqueNormal)
        {
            NaveContexto->GetRobotMeshComponent()->GetAnimInstance()->Montage_Play(NaveContexto->MontajeAtaqueNormal);
        }
    }

    // =========================================================================
    // PARTE 2: EL DAÑO (Tu código exacto de la esfera de colisión)
    // =========================================================================
    UWorld* const World = NaveContexto->GetWorld();
    if (World != nullptr)
    {
        FVector PosicionGolpe = NaveContexto->GetActorLocation() + (FireDirection * 150.0f);
        float RadioGolpe = 200.0f;
        float DanioBase = 25.0f;

        TArray<FOverlapResult> EnemigosGolpeados;
        FCollisionQueryParams ParametrosColision;
        ParametrosColision.AddIgnoredActor(NaveContexto); // No nos pegamos a nosotros mismos

        bool bHuboGolpe = World->OverlapMultiByChannel(
            EnemigosGolpeados,
            PosicionGolpe,
            FQuat::Identity,
            ECollisionChannel::ECC_Pawn,
            FCollisionShape::MakeSphere(RadioGolpe),
            ParametrosColision
        );

        if (bHuboGolpe)
        {
            for (FOverlapResult& Overlap : EnemigosGolpeados)
            {
                AActor* ActorGolpeado = Overlap.GetActor();
                if (ActorGolpeado != nullptr)
                {
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

// --- PATRÓN DECORATOR ---
FDecoradorRecuperacionNave::FDecoradorRecuperacionNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
    if (Contexto && Contexto->ComponenteCombate) {
        Contexto->ComponenteCombate->VidaActual = Contexto->ComponenteCombate->VidaMaxima;
        Contexto->ComponenteCombate->EscudoActual = Contexto->ComponenteCombate->EscudoMaximo;
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("NAVE RECUPERO VIDA Y ESCUDO"));
    }
}

FDecoradorCuadrupleCanon::FDecoradorCuadrupleCanon(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado) { Contexto->TiempoDisparoCuadruple = 10.0f; }

void FDecoradorCuadrupleCanon::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
    if (NaveContexto->TiempoDisparoCuadruple > 0.0f)
    {
        UWorld* const World = NaveContexto->GetWorld();
        if (World != nullptr)
        {
            const FRotator FireRotation = FireDirection.Rotation();
            FActorSpawnParameters SpawnParams; SpawnParams.Owner = NaveContexto;
            float Sep1 = 15.0f, Sep2 = 45.0f;
            FVector Izq1 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, -Sep1, NaveContexto->GunOffset.Z));
            FVector Der1 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, Sep1, NaveContexto->GunOffset.Z));
            FVector Izq2 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, -Sep2, NaveContexto->GunOffset.Z));
            FVector Der2 = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, Sep2, NaveContexto->GunOffset.Z));
            World->SpawnActor<AGalagaModificadoMacProjectile>(Izq1, FireRotation, SpawnParams);
            World->SpawnActor<AGalagaModificadoMacProjectile>(Der1, FireRotation, SpawnParams);
            World->SpawnActor<AGalagaModificadoMacProjectile>(Izq2, FireRotation, SpawnParams);
            World->SpawnActor<AGalagaModificadoMacProjectile>(Der2, FireRotation, SpawnParams);
        }
    }
    else FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
}

FDecoradorBombasRacimo::FDecoradorBombasRacimo(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado) { Contexto->BombasRacimoRestantes += 6; }

void FDecoradorBombasRacimo::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
    if (NaveContexto->BombasRacimoRestantes > 0)
    {
        UWorld* const World = NaveContexto->GetWorld();
        if (World != nullptr)
        {
            const FRotator FireRotation = FireDirection.Rotation();
            FActorSpawnParameters SpawnParams; SpawnParams.Owner = NaveContexto;
            NaveContexto->BombasRacimoRestantes--;
            for (int i = 0; i < 8; i++) { FRotator RacimoRot = FireRotation; RacimoRot.Yaw += (45.0f * i); World->SpawnActor<AGalagaModificadoMacProjectile>(NaveContexto->GetActorLocation(), RacimoRot, SpawnParams); }
        }
    }
    else FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
}

FDecoradorSuperBuffoNave::FDecoradorSuperBuffoNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
    Contexto->VelocidadOriginalNave = Contexto->MoveSpeed;
    Contexto->MoveSpeed *= 2.0f; Contexto->GetCharacterMovement()->MaxFlySpeed = Contexto->MoveSpeed;
    Contexto->MultiplicadorDanio = 1.5f; Contexto->TiempoBuffoNave = 8.0f;
}

FDecoradorVelocidadDash::FDecoradorVelocidadDash(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
    Contexto->MoveSpeed = 800.0f; Contexto->GetCharacterMovement()->MaxWalkSpeed = Contexto->MoveSpeed; Contexto->TiempoBuffoRobot = 10.0f;
}

FDecoradorCortesDistancia::FDecoradorCortesDistancia(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado) { Contexto->TiempoCortesDistancia = 12.0f; }

void FDecoradorCortesDistancia::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
    if (NaveContexto->TiempoCortesDistancia > 0.0f)
    {
        UWorld* const World = NaveContexto->GetWorld();
        if (World != nullptr)
        {
            const FRotator FireRotation = FireDirection.Rotation();
            FActorSpawnParameters SpawnParams; SpawnParams.Owner = NaveContexto;
            FVector SpawnLocation = NaveContexto->GetActorLocation() + FireRotation.RotateVector(FVector(NaveContexto->GunOffset.X, 0.0f, NaveContexto->GunOffset.Z));
            World->SpawnActor<AGalagaModificadoMacProjectile>(SpawnLocation, FireRotation, SpawnParams);
        }
    }
    else FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
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
    Contexto->TiempoInmunidad = 8.0f; Contexto->MultiplicadorDanio = 2.0f;
}