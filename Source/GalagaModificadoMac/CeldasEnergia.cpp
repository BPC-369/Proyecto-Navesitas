#include "CeldasEnergia.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

ACeldaEnergia::ACeldaEnergia()
{
    PrimaryActorTick.bCanEverTick = false;   // Sin rotación para mantener el centrado

    RaizCelda = CreateDefaultSubobject<USceneComponent>(TEXT("RaizCelda"));
    RootComponent = RaizCelda;

    MallaCelda = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCelda"));
    MallaCelda->SetupAttachment(RaizCelda);

    // Tu malla original del OVNI
    static ConstructorHelpers::FObjectFinder<UStaticMesh> NuevaMalla(TEXT("/Game/Geometry/pawn/uploads_files_4331233_UFO+4.uploads_files_4331233_UFO+4"));
    if (NuevaMalla.Succeeded())
    {
        MallaCelda->SetStaticMesh(NuevaMalla.Object);
    }

    // Escala de la celda (3 veces el tamaño original)
    MallaCelda->SetWorldScale3D(FVector(3.0f));
    MallaCelda->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Vida = 400.0f;
    bDestruida = false;
    MiObservador = nullptr;

    // Explosión de agua
    static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionAsset(
        TEXT("/Game/FXVarietyPack/Particles/P_ky_waterBallHit")
    );
    if (ExplosionAsset.Succeeded()) ExplosionEffect = ExplosionAsset.Object;

    // Sonido de explosión
    static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundAsset(
        TEXT("/Game/music/explooosion")
    );
    if (ExplosionSoundAsset.Succeeded()) ExplosionSound = ExplosionSoundAsset.Object;

    ExplosionScale = 15.0f;   // Explosión grande
}

void ACeldaEnergia::BeginPlay()
{
    Super::BeginPlay();

    // --- Centrar la malla teniendo en cuenta la escala ---
    if (MallaCelda && MallaCelda->GetStaticMesh())
    {
        FBox BoundingBox = MallaCelda->GetStaticMesh()->GetBoundingBox();
        FVector CentroMalla = BoundingBox.GetCenter();
        // La escala afecta a la posición del centro, así que multiplicamos
        MallaCelda->SetRelativeLocation(-CentroMalla * MallaCelda->GetComponentScale());
    }
}

void ACeldaEnergia::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Sin rotación
}

void ACeldaEnergia::AsignarObservador(ICeldaObserver* Observador)
{
    MiObservador = Observador;
}

float ACeldaEnergia::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bDestruida) return 0.0f;

    Vida -= DamageAmount;
    if (Vida <= 0)
    {
        Vida = 0;
        bDestruida = true;
        if (MiObservador)
        {
            MiObservador->NotificarCeldaDestruida(this);
        }
        Destroy();
    }
    return DamageAmount;
}

void ACeldaEnergia::Destroyed()
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