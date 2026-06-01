#include "BossProjectile.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h" // <-- Incluimos la librería de Niagara
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/DamageType.h"

ABossProjectile::ABossProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    ColisionEsfera = CreateDefaultSubobject<USphereComponent>(TEXT("ColisionEsfera"));
    ColisionEsfera->InitSphereRadius(20.0f);
    ColisionEsfera->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = ColisionEsfera;

    // --- INICIO CONFIGURACIÓN NIAGARA ---
    EfectoNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EfectoNiagara"));
    EfectoNiagara->SetupAttachment(RootComponent);

    // Cargamos el asset de Niagara directamente usando la ruta de tus carpetas
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraAsset(TEXT("/Game/Basic_VFX/Niagara/NS_Basic_9.NS_Basic_9"));
    if (NiagaraAsset.Succeeded())
    {
        EfectoNiagara->SetAsset(NiagaraAsset.Object);
    }
    // --- FIN CONFIGURACIÓN NIAGARA ---

    ComponenteMovimiento = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ComponenteMovimiento"));
    ComponenteMovimiento->UpdatedComponent = ColisionEsfera;
    ComponenteMovimiento->ProjectileGravityScale = 0.0f;

    DanoProyectil = 50.0f;
    InitialLifeSpan = 20.0f;
}

void ABossProjectile::ConfigurarProyectil(float NuevaEscala, float NuevoDano, float NuevaVelocidad)
{
    SetActorScale3D(FVector(NuevaEscala));
    DanoProyectil = NuevoDano;
    ComponenteMovimiento->InitialSpeed = NuevaVelocidad;
    ComponenteMovimiento->MaxSpeed = NuevaVelocidad;

    // --- FIX 2: ¡Obligamos a la bala a moverse hacia adelante! ---
    ComponenteMovimiento->Velocity = GetActorForwardVector() * NuevaVelocidad;
}

void ABossProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (OtherActor && OtherActor != this && OtherActor != GetInstigator())
    {
        UGameplayStatics::ApplyDamage(
            OtherActor,
            DanoProyectil,
            GetInstigatorController(),
            this,
            UDamageType::StaticClass()
        );

        Destroy();
    }
}