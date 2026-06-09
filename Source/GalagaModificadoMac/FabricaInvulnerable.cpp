#include "FabricaInvulnerable.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "FabricaNaves.h"

AFabricaInvulnerable::AFabricaInvulnerable()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
    ParticleComponent->SetupAttachment(RootComponent);
    ParticleComponent->bAutoActivate = true;
}

void AFabricaInvulnerable::BeginPlay()
{
    Super::BeginPlay();

    if (ParticleComponent)
    {
        // Aplicar la altura configurada (desplazamiento vertical del efecto)
        ParticleComponent->SetRelativeLocation(FVector(0.0f, 0.0f, AlturaZ));

        UParticleSystem* Tornado = LoadObject<UParticleSystem>(nullptr, TEXT("/Game/FXVarietyPack/Particles/P_ky_darkStorm"));
        if (Tornado)
        {
            ParticleComponent->SetTemplate(Tornado);
            ParticleComponent->SetWorldScale3D(FVector(15.0f));   // Escala imponente
            ParticleComponent->Activate(true);
            UE_LOG(LogTemp, Warning, TEXT("FabricaInvulnerable: Tornado oscuro cargado, altura = %f"), AlturaZ);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("FabricaInvulnerable: No se encontró P_ky_darkStorm"));
        }
    }

    GetWorldTimerManager().SetTimer(TimerHandle_Spawn, this, &AFabricaInvulnerable::SpawnNaves, IntervaloSpawn, true);
}

void AFabricaInvulnerable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float AFabricaInvulnerable::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    return 0.0f;
}

void AFabricaInvulnerable::SpawnNaves()
{
    if (!GetWorld()) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    FVector PosicionBase = GetActorLocation();
    FRotator RotacionBase = GetActorRotation();

    for (int32 i = 0; i < NavesPorSpawn; i++)
    {
        // Rango cercano al portal
        FVector Offset = FVector(FMath::RandRange(-100.0f, 100.0f), FMath::RandRange(-100.0f, 100.0f), 0.0f);
        FVector PosicionSpawn = PosicionBase + Offset;

        FabricaNaves::TipoNave Tipo = FMath::RandBool() ? FabricaNaves::COMUN : FabricaNaves::KAMIKASE;
        FabricaNaves::CrearNave(Tipo, GetWorld(), PosicionSpawn, RotacionBase);
    }
}