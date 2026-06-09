#include "BossEstatico.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "CeldasEnergia.h"
#include "AttackStrategies.h"
#include "GalagaModificadoMacGameMode.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "FabricaInvulnerable.h"
#include "GalagaModificadoMacGameMode.h"
#include "BossAttackBuilder.h"

ABossEstatico::ABossEstatico()
{
    PrimaryActorTick.bCanEverTick = true;

    CapsulaColision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsulaColision"));
    RootComponent = CapsulaColision;
    CapsulaColision->SetCapsuleSize(800.0f, 1200.0f);
    CapsulaColision->SetCollisionProfileName(TEXT("Pawn"));

    MallaJefe = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaJefe"));
    MallaJefe->SetupAttachment(RootComponent);
    MallaJefe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaJefeNueva(
        TEXT("/Game/Geometry/jefeMalla/Meshy_AI_War_Machine_Chief_Ort_0603010709_texture.Meshy_AI_War_Machine_Chief_Ort_0603010709_texture")
    );
    if (MallaJefeNueva.Succeeded())
    {
        MallaJefe->SetStaticMesh(MallaJefeNueva.Object);
    }

    MallaJefe->SetWorldScale3D(FVector(30.0f, 30.0f, 30.0f));

    VidaMaxima = 8000.0f;
    VidaJefe = VidaMaxima;
    CeldasActivas = 0;
    bEscudoInmune = true;
    EstrategiaActual = nullptr;

    bFuriaCeldas = false;
    bFuriaVida = false;
    IntervaloCambio = 5.0f;
    TiempoAcumuladoCambio = 0.0f;
    MultiplicadorDano = 1.0f;
    MultiplicadorVelocidad = 1.0f;

    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false;

    EscudoPSC = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EscudoPSC"));
    EscudoPSC->SetupAttachment(RootComponent);
    EscudoPSC->bAutoActivate = false;
    EscudoPSC->SetRelativeLocation(FVector::ZeroVector);

    static ConstructorHelpers::FObjectFinder<UParticleSystem> EscudoAsset(
        TEXT("/Game/FXVarietyPack/Particles/P_ky_magicCircle1")
    );
    if (EscudoAsset.Succeeded()) EscudoEffect = EscudoAsset.Object;

    static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEscudoAsset(
        TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Destruction/P_Destruction_Electric.P_Destruction_Electric")
    );
    if (ExplosionEscudoAsset.Succeeded()) ExplosionEscudo = ExplosionEscudoAsset.Object;

    static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionMuerteAsset(
        TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Big_A")
    );
    if (ExplosionMuerteAsset.Succeeded()) ExplosionMuerte = ExplosionMuerteAsset.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> RisaAsset(
        TEXT("/Game/music/risaBoss.risaBoss")
    );
    if (RisaAsset.Succeeded()) SoundRisa = RisaAsset.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> SonidoExplosionEscudoAsset(
        TEXT("/Game/music/explooosion.explooosion")
    );
    if (SonidoExplosionEscudoAsset.Succeeded()) SoundExplosionEscudo = SonidoExplosionEscudoAsset.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> SonidoExplosionMuerteAsset(
        TEXT("/Game/music/xplosionBoss.xplosionBoss")
    );
    if (SonidoExplosionMuerteAsset.Succeeded()) SoundExplosionMuerte = SonidoExplosionMuerteAsset.Object;

    IntervaloRisa = 15.0f;
    EscudoEffectScale = 40.0f;
    ExplosionEscudoScale = 2.0f;
    ExplosionMuerteScale = 30.0f;
}

void ABossEstatico::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> CeldasEnMapa;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACeldaEnergia::StaticClass(), CeldasEnMapa);
    CeldasActivas = CeldasEnMapa.Num();

    for (AActor* ActorCelda : CeldasEnMapa)
    {
        ACeldaEnergia* Celda = Cast<ACeldaEnergia>(ActorCelda);
        if (Celda) Celda->AsignarObservador(this);
    }

    if (EscudoPSC && EscudoEffect && CeldasActivas > 0)
    {
        EscudoPSC->SetTemplate(EscudoEffect);
        EscudoPSC->SetWorldScale3D(FVector(EscudoEffectScale, EscudoEffectScale, EscudoEffectScale * 1.0f));
        float HalfHeight = CapsulaColision->GetScaledCapsuleHalfHeight();
        float OffsetDesdeSuelo = -1850.0f;
        EscudoPSC->SetRelativeLocation(FVector(0.0f, 0.0f, -HalfHeight + OffsetDesdeSuelo));
        EscudoPSC->Activate(true);
        EscudoPSC->SetCullDistance(0.0f);
    }

    StartLaugh();
    CambiarEstrategia(new FAtaqueParedStrategy());
}

void ABossEstatico::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    StopLaugh();
    if (EstrategiaActual)
    {
        delete EstrategiaActual;
        EstrategiaActual = nullptr;
    }
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Lluvia);
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Risa);
    }
}

void ABossEstatico::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsPendingKill()) return;

    if (bFuriaCeldas)
    {
        TiempoAcumuladoCambio += DeltaTime;
        if (TiempoAcumuladoCambio >= IntervaloCambio)
        {
            SeleccionarAtaqueAleatorioFuria();
            TiempoAcumuladoCambio = 0.0f;
        }
    }

    if (EstrategiaActual) EstrategiaActual->Ejecutar(this, DeltaTime);
}

float ABossEstatico::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bEscudoInmune && CeldasActivas > 0) return 0.0f;

    VidaJefe -= DamageAmount;

    if (!bFuriaVida && VidaJefe <= VidaMaxima * 0.5f)
    {
        bFuriaVida = true;
        IntervaloCambio = 2.0f;
        MultiplicadorDano = 1.5f;
        MultiplicadorVelocidad = 1.5f;
        TiempoAcumuladoCambio = 0.0f;

        // Iniciar la lluvia de proyectiles (solo una vez, al entrar en furia por vida)
        IniciarLluvia();
    }

    if (VidaJefe <= 0) Destroy();
    return DamageAmount;
}

void ABossEstatico::NotificarCeldaDestruida(ACeldaEnergia* CeldaQueMurio)
{
    CeldasActivas--;

    // --- Spawn de un Cuartel Invulnerable a ras de suelo ---
    if (AGalagaModificadoMacGameMode* GM = GetWorld()->GetAuthGameMode<AGalagaModificadoMacGameMode>())
    {
        if (GM->ListaFabricas.Num() < 3)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            float HalfHeight = CapsulaColision->GetScaledCapsuleHalfHeight();
            FVector PosicionSuelo = GetActorLocation() - FVector(0.0f, 0.0f, HalfHeight);

            FVector PosicionFabrica = PosicionSuelo + FVector(
                FMath::RandRange(-20000.0f, 20000.0f),
                FMath::RandRange(-20000.0f, 20000.0f),
                -1500.0f
            );

            AFabricaInvulnerable* Cuartel = GetWorld()->SpawnActor<AFabricaInvulnerable>(
                AFabricaInvulnerable::StaticClass(),
                PosicionFabrica,
                FRotator::ZeroRotator,
                SpawnParams
                );

            if (Cuartel)
            {
                GM->ListaFabricas.Add(Cuartel);
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange,
                    FString::Printf(TEXT("¡Cuartel invulnerable activado! (%d/3)"), GM->ListaFabricas.Num()));
            }
        }
    }

    // --- Lógica original de cambio de estrategia, escudo, música ---
    if (!bFuriaCeldas)
    {
        if (CeldasActivas == 2) CambiarEstrategia(new FAtaqueOndaStrategy());
        else if (CeldasActivas == 1) CambiarEstrategia(new FAtaqueLatigoStrategy());
    }

    if (CeldasActivas <= 0)
    {
        bEscudoInmune = false;
        StopLaugh();

        if (EscudoPSC && EscudoPSC->IsActive()) EscudoPSC->Deactivate();

        if (ExplosionEscudo)
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEscudo, GetActorLocation(), GetActorRotation(), FVector(ExplosionEscudoScale), true);
        if (SoundExplosionEscudo)
            UGameplayStatics::PlaySoundAtLocation(this, SoundExplosionEscudo, GetActorLocation());

        AGalagaModificadoMacGameMode* GM = Cast<AGalagaModificadoMacGameMode>(GetWorld()->GetAuthGameMode());
        if (GM) GM->CambiarMusicaJefe(1);

        if (!bFuriaCeldas)
        {
            bFuriaCeldas = true;
            IntervaloCambio = 5.0f;
            TiempoAcumuladoCambio = 0.0f;
            SeleccionarAtaqueAleatorioFuria();
        }
    }
}

void ABossEstatico::SeleccionarAtaqueAleatorioFuria()
{
    int32 Dado = FMath::RandRange(1, 3);
    switch (Dado)
    {
    case 1: CambiarEstrategia(new FAtaqueParedStrategy()); break;
    case 2: CambiarEstrategia(new FAtaqueOndaStrategy()); break;
    case 3: CambiarEstrategia(new FAtaqueLatigoStrategy()); break;
    }
}

void ABossEstatico::CambiarEstrategia(IAttackStrategy* NuevaEstrategia)
{
    if (EstrategiaActual) delete EstrategiaActual;
    EstrategiaActual = NuevaEstrategia;
}

void ABossEstatico::Destroyed()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Lluvia);
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Risa);
    }

    if (ExplosionMuerte)
    {
        float HalfHeight = CapsulaColision->GetScaledCapsuleHalfHeight();
        FVector BaseLocation = GetActorLocation() - FVector(0.0f, 0.0f, HalfHeight);
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionMuerte, BaseLocation, GetActorRotation(), FVector(ExplosionMuerteScale), true);
    }
    if (SoundExplosionMuerte)
        UGameplayStatics::PlaySoundAtLocation(this, SoundExplosionMuerte, GetActorLocation());

    Super::Destroyed();
}

// ========== RISA PERIÓDICA ==========
void ABossEstatico::StartLaugh()
{
    if (!SoundRisa || IntervaloRisa <= 0.0f) return;
    PlayLaugh();
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_Risa, this, &ABossEstatico::PlayLaugh, IntervaloRisa, true);
}

void ABossEstatico::PlayLaugh()
{
    if (SoundRisa && AudioComponent)
    {
        AudioComponent->SetSound(SoundRisa);
        AudioComponent->Play();
    }
}

void ABossEstatico::StopLaugh()
{
    if (GetWorld()) GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Risa);
    if (AudioComponent && AudioComponent->IsPlaying()) AudioComponent->Stop();
}

// ========== LLUVIA DE PROYECTILES (furia por vida) ==========
void ABossEstatico::IniciarLluvia()
{
    // Solo si la furia por vida está activa y el jefe no está siendo destruido
    if (!bFuriaVida || IntervaloLluvia <= 0.0f || IsPendingKill()) return;

    EjecutarLluvia();
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_Lluvia, this, &ABossEstatico::EjecutarLluvia, IntervaloLluvia, true);
    }
}

void ABossEstatico::EjecutarLluvia()
{
    // No ejecutar si el jefe ya ha muerto o está siendo destruido
    if (!bFuriaVida || IsPendingKill() || !GetWorld()) return;

    BossAttackBuilder Builder(GetWorld(), this);
    Builder.SetDano(150.0f).SetVelocidad(1000.0f).SetEscala(2.0f);
    Builder.ConstruirLluviaTechada(GetActorLocation(), 10000.0f, 200, 3000.0f);
}