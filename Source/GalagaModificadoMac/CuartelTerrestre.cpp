#include "CuartelTerrestre.h"
#include "Components/StaticMeshComponent.h"
#include "ComponenteCombate.h"
#include "FabricaRobots.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "Engine/World.h"

ACuartelTerrestre::ACuartelTerrestre()
{
    PrimaryActorTick.bCanEverTick = false;

    MallaCuartel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCuartel"));
    RootComponent = MallaCuartel;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaBase(TEXT("/Game/Modelos/Edificios/edificionuevo/Meshy_AI_Twin_Spire_Nexus_0603022815_texture"));
    if (MallaBase.Succeeded())
        MallaCuartel->SetStaticMesh(MallaBase.Object);

    // Tamaño colosal
    MallaCuartel->SetWorldScale3D(FVector(30.0f, 30.0f, 20.0f));

    MallaCuartel->SetCollisionProfileName(TEXT("Pawn"));

    ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));
    if (ComponenteCombate)
    {
        ComponenteCombate->VidaMaxima = VidaMaxima;
        ComponenteCombate->VidaActual = VidaMaxima;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

    static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionAsset(
        TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Big_A"));
    if (ExplosionAsset.Succeeded())
        ExplosionEffect = ExplosionAsset.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> SonidoExplosion(TEXT("/Game/music/explooosion"));
    if (SonidoExplosion.Succeeded())
        ExplosionSound = SonidoExplosion.Object;
}

void ACuartelTerrestre::BeginPlay()
{
    Super::BeginPlay();

    if (ComponenteCombate)
    {
        ComponenteCombate->VidaMaxima = VidaMaxima;
        ComponenteCombate->VidaActual = VidaMaxima;
    }

    GetWorld()->GetTimerManager().SetTimer(TimerHandle_Spawn, this,
        &ACuartelTerrestre::SpawnOleada, IntervaloAparicion, true);
}

void ACuartelTerrestre::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld())
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Spawn);
    Super::EndPlay(EndPlayReason);
}

void ACuartelTerrestre::SpawnOleada()
{
    if (!GetWorld() || IsPendingKill())
    {
        if (GetWorld())
            GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Spawn);
        return;
    }

    if (UnidadesGeneradas >= MaxUnidades)
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Spawn);
        return;
    }

    int32 Restantes = MaxUnidades - UnidadesGeneradas;
    int32 ASpawnear = FMath::Min(UnidadesPorOleada, Restantes);

    static const FabricaRobots::TipoRobot TiposDisponibles[] = {
        FabricaRobots::LIDER,
        FabricaRobots::MEDICO,
        FabricaRobots::RAZO,
        FabricaRobots::RZ,
        FabricaRobots::RZ_D,
        FabricaRobots::FRANCOTIRADOR
    };
    static const int32 NumTipos = sizeof(TiposDisponibles) / sizeof(TiposDisponibles[0]);

    for (int32 i = 0; i < ASpawnear; i++)
    {
        if (IsPendingKill()) break;

        FabricaRobots::TipoRobot Tipo = TiposDisponibles[FMath::RandRange(0, NumTipos - 1)];

        FVector Base = GetActorLocation();
        FVector Desplazamiento = FMath::VRand() * FMath::FRandRange(0.0f, RadioSpawn);
        Desplazamiento.Z = 0.0f;
        FVector SpawnLoc = Base + Desplazamiento;
        SpawnLoc.Z += 100.0f;

        FRotator SpawnRot = FRotator::ZeroRotator;

        AEnemigoTerrestre* Robot = FabricaRobots::CrearRobot(Tipo, GetWorld(), SpawnLoc, SpawnRot);
        if (Robot)
        {
            UnidadesGeneradas++;
        }
    }
}

float ACuartelTerrestre::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ComponenteCombate)
    {
        DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);
        if (ComponenteCombate->VidaActual <= 0.0f)
        {
            Destroy();
        }
    }
    return DanioReal;
}

void ACuartelTerrestre::Destroyed()
{
    if (GetWorld())
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Spawn);

    if (ExplosionEffect)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect,
            GetActorLocation(), GetActorRotation(), FVector(3.0f), true);
    if (ExplosionSound)
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

    Super::Destroyed();
}