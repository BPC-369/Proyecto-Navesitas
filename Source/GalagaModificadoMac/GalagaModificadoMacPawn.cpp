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

// Para las barras de vida
#include "BossEstatico.h"
#include "NaveComando.h"
#include "CeldasEnergia.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/PanelWidget.h"
#include "EngineUtils.h"
#include "Components/VerticalBoxSlot.h"

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

    static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCubo(TEXT("StaticMesh'/Game/Geometry/pawn/pawn09.pawn09'"));
    if (FormaCubo.Succeeded()) RopaCubo = FormaCubo.Object;

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

    MoveSpeed = 1700.0f;
    VelocidadOriginalNave = 1700.0f;

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

    // Carga de widgets para las barras de vida y crosshair
    static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarBP(TEXT("/Game/Blueprints/WBP_EnemyHealthBar"));
    if (HealthBarBP.Succeeded()) EnemyHealthBarClass = HealthBarBP.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> EnemyListBP(TEXT("/Game/Blueprints/WBP_EnemyList"));
    if (EnemyListBP.Succeeded()) EnemyListClass = EnemyListBP.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> BossBarBP(TEXT("/Game/Blueprints/WBP_BossHealth"));
    if (BossBarBP.Succeeded()) BossHealthClass = BossBarBP.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> CrosshairBP(TEXT("/Game/Blueprints/WBP_Crosshair"));
    if (CrosshairBP.Succeeded()) CrosshairClass = CrosshairBP.Class;
}

void AGalagaModificadoMacPawn::BeginPlay()
{
    Super::BeginPlay();
    if (ShipMeshComponent)
        ShipMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    ConvertirEnNave();

    // Crear widget contenedor de la lista de enemigos (lateral izquierdo)
    if (EnemyListClass)
    {
        EnemyListWidget = CreateWidget<UUserWidget>(GetWorld(), EnemyListClass);
        if (EnemyListWidget)
            EnemyListWidget->AddToViewport(10); // ZOrder bajo
    }

    // Crear widget de la barra del jefe (centro superior)
    if (BossHealthClass)
    {
        BossHealthWidget = CreateWidget<UUserWidget>(GetWorld(), BossHealthClass);
        if (BossHealthWidget)
            BossHealthWidget->AddToViewport();
    }

    // Crear y mostrar el crosshair
    if (CrosshairClass)
    {
        UUserWidget* CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairClass);
        if (CrosshairWidget)
            CrosshairWidget->AddToViewport(100); // ZOrder alto, siempre visible
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
        FireShot(GetActorForwardVector());

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

    UpdateHealthBars();
}

void AGalagaModificadoMacPawn::FireShot(FVector FireDirection)
{
    if (EstadoActual != nullptr)
        EstadoActual->EjecutarAtaque(this, FireDirection);

    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AGalagaModificadoMacPawn::ShotTimerExpired, FireRate);

    if (FireSound != nullptr)
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
}

void AGalagaModificadoMacPawn::ShotTimerExpired()
{
    bCanFire = true;
}

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
    if (EstadoActual != nullptr) delete EstadoActual;
    EstadoActual = NuevoEstado;
}

void AGalagaModificadoMacPawn::ConvertirEnNave()
{
    if (RopaNave != nullptr) ShipMeshComponent->SetStaticMesh(RopaNave);
    if (GetCharacterMovement()) GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AGalagaModificadoMacPawn::ConvertirEnRobot()
{
    if (RopaCubo != nullptr) ShipMeshComponent->SetStaticMesh(RopaCubo);
    if (GetCharacterMovement()) GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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
    AGalagaModificadoMacGameMode* GM = Cast<AGalagaModificadoMacGameMode>(GetWorld()->GetAuthGameMode());
    if (GM) GM->DetenerMusica();

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
        if (GameOverWidget) GameOverWidget->AddToViewport();
    }
}

float AGalagaModificadoMacPawn::GetVidaActual() const
{
    if (ComponenteCombate) return ComponenteCombate->VidaActual;
    return 0.0f;
}

float AGalagaModificadoMacPawn::GetVidaMaxima() const
{
    if (ComponenteCombate) return ComponenteCombate->VidaMaxima;
    return 1.0f;
}

// ========== BARRAS DE VIDA EN HUD ==========
void AGalagaModificadoMacPawn::UpdateHealthBars()
{
    TArray<AActor*> EnemigosActuales;

    for (TActorIterator<ANaveComando> It(GetWorld()); It; ++It)
        if (*It && !(*It)->IsPendingKill()) EnemigosActuales.Add(*It);

    for (TActorIterator<ACeldaEnergia> It(GetWorld()); It; ++It)
        if (*It && !(*It)->IsPendingKill()) EnemigosActuales.Add(*It);

    // Eliminar widgets de enemigos muertos
    TArray<AActor*> ParaEliminar;
    for (auto& Par : EnemyHealthWidgets)
    {
        if (!Par.Key || Par.Key->IsPendingKill() || !EnemigosActuales.Contains(Par.Key))
        {
            if (Par.Value) Par.Value->RemoveFromParent();
            ParaEliminar.Add(Par.Key);
        }
    }
    for (AActor* Key : ParaEliminar) EnemyHealthWidgets.Remove(Key);

    // Crear widgets para nuevos enemigos
    for (AActor* Enemy : EnemigosActuales)
    {
        if (!EnemyHealthWidgets.Contains(Enemy))
        {
            UUserWidget* Widget = CreateHealthBarForEnemy(Enemy);
            EnemyHealthWidgets.Add(Enemy, Widget);
        }
    }

    // Panel de lista
    UPanelWidget* ListaPanel = nullptr;
    if (EnemyListWidget)
        ListaPanel = Cast<UPanelWidget>(EnemyListWidget->GetWidgetFromName(TEXT("EnemyListPanel")));

    if (ListaPanel)
    {
        EnemigosActuales.Sort([](const AActor& A, const AActor& B) {
            if (A.IsA(ANaveComando::StaticClass()) && !B.IsA(ANaveComando::StaticClass())) return true;
            if (!A.IsA(ANaveComando::StaticClass()) && B.IsA(ANaveComando::StaticClass())) return false;
            return false;
            });

        ListaPanel->ClearChildren();
        for (AActor* Enemy : EnemigosActuales)
        {
            UUserWidget* Widget = EnemyHealthWidgets.Contains(Enemy) ? EnemyHealthWidgets[Enemy] : nullptr;
            if (Widget)
            {
                ListaPanel->AddChild(Widget);
                if (UVerticalBoxSlot* Slot = Cast<UVerticalBoxSlot>(Widget->Slot))
                {
                    Slot->SetPadding(FMargin(0, 0, 0, 10)); // 10 píxeles de separación inferior
                }
                float Percent = 1.0f;
                if (ANaveComando* N = Cast<ANaveComando>(Enemy))
                {
                    UComponenteCombate* Comp = N->GetComponenteCombate();
                    if (Comp) Percent = Comp->VidaActual / Comp->VidaMaxima;
                }
                else if (ACeldaEnergia* C = Cast<ACeldaEnergia>(Enemy))
                {
                    Percent = C->GetVida() / C->GetVidaMaxima();
                }

                UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("ProgressBar_0")));
                if (Bar) Bar->SetPercent(Percent);
            }
        }
    }

    // Barra del jefe (centro superior)
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
                Bar->SetPercent(Percent);
                Bar->SetVisibility(ESlateVisibility::Visible);
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
    UWorld* const World = NaveContexto->GetWorld();
    if (World != nullptr)
    {
        FVector PosicionGolpe = NaveContexto->GetActorLocation() + (FireDirection * 150.0f);
        float RadioGolpe = 200.0f;
        float DanioBase = 25.0f;

        TArray<FOverlapResult> EnemigosGolpeados;
        FCollisionQueryParams ParametrosColision;
        ParametrosColision.AddIgnoredActor(NaveContexto);

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

FDecoradorCuadrupleCanon::FDecoradorCuadrupleCanon(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
    Contexto->TiempoDisparoCuadruple = 10.0f;
}

void FDecoradorCuadrupleCanon::EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection)
{
    if (NaveContexto->TiempoDisparoCuadruple > 0.0f)
    {
        UWorld* const World = NaveContexto->GetWorld();
        if (World != nullptr)
        {
            const FRotator FireRotation = FireDirection.Rotation();
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = NaveContexto;

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

FDecoradorBombasRacimo::FDecoradorBombasRacimo(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
    Contexto->BombasRacimoRestantes += 6;
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
    else FDecoradorBonificacion::EjecutarAtaque(NaveContexto, FireDirection);
}

FDecoradorSuperBuffoNave::FDecoradorSuperBuffoNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto) : FDecoradorBonificacion(Estado)
{
    Contexto->VelocidadOriginalNave = Contexto->MoveSpeed;
    Contexto->MoveSpeed *= 2.0f;
    Contexto->GetCharacterMovement()->MaxFlySpeed = Contexto->MoveSpeed;
    Contexto->MultiplicadorDanio = 1.5f;
    Contexto->TiempoBuffoNave = 8.0f;
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
    Contexto->TiempoInmunidad = 8.0f;
    Contexto->MultiplicadorDanio = 2.0f;
}