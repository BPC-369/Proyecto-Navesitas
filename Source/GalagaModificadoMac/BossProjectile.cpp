#include "BossProjectile.h"
#include "BossEstatico.h"
#include "CeldasEnergia.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

ABossProjectile::ABossProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    InitialLifeSpan = 3.5f;                     // Vida máxima de 3 segundos

    Colisionador = CreateDefaultSubobject<USphereComponent>(TEXT("Colisionador"));
    Colisionador->InitSphereRadius(25.0f);
    RootComponent = Colisionador;

    Colisionador->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Colisionador->SetCollisionResponseToAllChannels(ECR_Overlap);
    Colisionador->OnComponentBeginOverlap.AddDynamic(this, &ABossProjectile::AlEntrarEnColision);

    VFXNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFXNiagara"));
    VFXNiagara->SetupAttachment(RootComponent);
    VFXNiagara->bAutoActivate = true;

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemFinder(
        TEXT("/Game/Basic_VFX/Niagara/NS_Basic_9.NS_Basic_9")
    );
    if (NiagaraSystemFinder.Succeeded())
    {
        VFXNiagara->SetAsset(NiagaraSystemFinder.Object);
    }

    ComponenteMovimiento = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ComponenteMovimiento"));
    ComponenteMovimiento->InitialSpeed = 1500.0f;
    ComponenteMovimiento->MaxSpeed = 1500.0f;
    ComponenteMovimiento->ProjectileGravityScale = 0.0f;

    Dano = 200.0f;
    MaxTravelDistance = 10000.0f;                // Destruir si supera esta distancia
}

void ABossProjectile::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();

    if (VFXNiagara)
    {
        VFXNiagara->Activate(true);
    }
}

void ABossProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Destruir si ha viajado más de MaxTravelDistance desde el origen
    if (FVector::DistSquared(GetActorLocation(), SpawnLocation) > FMath::Square(MaxTravelDistance))
    {
        Destroy();
    }
}

void ABossProjectile::ConfigurarProyectil(float NuevaVelocidad, float NuevoDano, FVector Escala)
{
    if (ComponenteMovimiento)
    {
        ComponenteMovimiento->InitialSpeed = NuevaVelocidad;
        ComponenteMovimiento->MaxSpeed = NuevaVelocidad;
    }
    Dano = NuevoDano;
    SetActorScale3D(Escala);                    // Escala uniforme, sin deformar
}

void ABossProjectile::SetDireccion(FVector Direccion)
{
    if (ComponenteMovimiento)
    {
        ComponenteMovimiento->Velocity = Direccion.GetSafeNormal() * ComponenteMovimiento->InitialSpeed;
    }
}

void ABossProjectile::AlEntrarEnColision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetInstigator()) return;
    if (OtherActor->IsA(ABossEstatico::StaticClass()) ||
        OtherActor->IsA(ABossProjectile::StaticClass()) ||
        OtherActor->IsA(ACeldaEnergia::StaticClass())) return;

    if (bRalentiza)
    {
        AplicarRalentizacion(OtherActor);
    }

    UGameplayStatics::ApplyDamage(OtherActor, Dano, GetInstigatorController(), this, UDamageType::StaticClass());
    Destroy();
}

void ABossProjectile::AplicarRalentizacion(AActor* Victima)
{
    ACharacter* PlayerChar = Cast<ACharacter>(Victima);
    if (PlayerChar)
    {
        if (auto* MoveComp = PlayerChar->GetCharacterMovement())
        {
            float VelocidadOriginal = MoveComp->MaxWalkSpeed;
            MoveComp->MaxWalkSpeed *= 0.4f;

            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [MoveComp, VelocidadOriginal]()
                {
                    if (MoveComp)
                    {
                        MoveComp->MaxWalkSpeed = VelocidadOriginal;
                    }
                }, 2.0f, false);
        }
    }
}