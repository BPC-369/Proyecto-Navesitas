#include "NaveEnemigoAereo.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ComponenteCombate.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"

ANaveEnemigoAereo::ANaveEnemigoAereo()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* EscenaRaiz = CreateDefaultSubobject<USceneComponent>(TEXT("EscenaRaiz"));
    RootComponent = EscenaRaiz;

    MallaEnemiga = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEnemiga"));
    MallaEnemiga->SetupAttachment(RootComponent);
    MallaEnemiga->SetNotifyRigidBodyCollision(true);
    MallaEnemiga->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));
    if (ComponenteCombate)
    {
        ComponenteCombate->VidaMaxima = 100.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

    Velocidad = 500.0f;
    DanioAtaque = 1000.0f;
    FrecuenciaAtaque = 2.0f;
    bTieneEscudo = false;

    // --- Explosión por defecto (pequeña) para todas las naves ---
    // Reemplaza esta ruta con la de tu explosión pequeña (usa Copy Reference)
    RutaExplosion = TEXT("ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Side.P_Explosion_Side'");
    ExplosionScale = 1.0f;

    // Si quieres sonido para todas, define la ruta aquí
    // RutaSonidoExplosion = TEXT("/Game/Audio/ExplosionSound");
}

void ANaveEnemigoAereo::BeginPlay()
{
    Super::BeginPlay();

    // Cargar el efecto de explosión si hay ruta definida y aún no se ha cargado
    if (!ExplosionEffect && !RutaExplosion.IsEmpty())
    {
        ExplosionEffect = LoadObject<UParticleSystem>(nullptr, *RutaExplosion);
    }

    if (!ExplosionSound && !RutaSonidoExplosion.IsEmpty())
    {
        ExplosionSound = LoadObject<USoundBase>(nullptr, *RutaSonidoExplosion);
    }
}

void ANaveEnemigoAereo::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    Volar(DeltaSeconds);
}

void ANaveEnemigoAereo::Volar(float DeltaSeconds)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        FVector Direccion = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FVector Desplazamiento = Direccion * Velocidad * DeltaSeconds;
        AddActorWorldOffset(Desplazamiento, true);
        SetActorRotation(Direccion.Rotation());
    }
}

void ANaveEnemigoAereo::Atacar()
{
}

float ANaveEnemigoAereo::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ComponenteCombate)
    {
        DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);
    }
    return DanioReal;
}

void ANaveEnemigoAereo::Destroyed()
{
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionEffect,
            GetActorLocation(),
            GetActorRotation(),
            FVector(ExplosionScale),
            true
        );
    }

    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    }

    Super::Destroyed();
}