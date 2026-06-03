#include "BossProjectile.h"
#include "BossEstatico.h"
#include "CeldasEnergia.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraSystem.h"                          // Tipo del asset
#include "UObject/ConstructorHelpers.h"

ABossProjectile::ABossProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    // Colisionador esférico invisible como raíz
    Colisionador = CreateDefaultSubobject<USphereComponent>(TEXT("Colisionador"));
    Colisionador->InitSphereRadius(25.0f);          // Ajusta el tamaño según tu VFX
    RootComponent = Colisionador;

    Colisionador->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Colisionador->SetCollisionResponseToAllChannels(ECR_Overlap);
    Colisionador->OnComponentBeginOverlap.AddDynamic(this, &ABossProjectile::AlEntrarEnColision);

    // Componente de VFX Niagara
    VFXNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFXNiagara"));
    VFXNiagara->SetupAttachment(RootComponent);
    VFXNiagara->bAutoActivate = true;               // Se reproduce automáticamente al aparecer

    // Cargar el sistema Niagara desde la ruta que copiaste
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemFinder(
        TEXT("/Game/Basic_VFX/Niagara/NS_Basic_9.NS_Basic_9")
    );
    if (NiagaraSystemFinder.Succeeded())
    {
        VFXNiagara->SetAsset(NiagaraSystemFinder.Object);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No se pudo cargar el Niagara System NS_Basic_9"));
    }

    // Movimiento
    ComponenteMovimiento = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ComponenteMovimiento"));
    ComponenteMovimiento->InitialSpeed = 1500.0f;
    ComponenteMovimiento->MaxSpeed = 1500.0f;
    ComponenteMovimiento->ProjectileGravityScale = 0.0f;

    Dano = 20.0f;
}

void ABossProjectile::BeginPlay()
{
    Super::BeginPlay();
    SpawnLocation = GetActorLocation();

    // Activar el Niagara explícitamente (por si acaso)
    if (VFXNiagara)
    {
        VFXNiagara->Activate(true);
    }
}

void ABossProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Destrucción por distancia
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
    SetActorScale3D(Escala);                        // Escala también afecta al VFX
}

void ABossProjectile::HabilitarEfectoOnda(float EscalaExtra, float bActivar)
{
    // Puedes modificar parámetros del Niagara aquí si lo deseas
    // Por ahora solo ajustamos escala
    SetActorScale3D(FVector(EscalaExtra, 0.5f, 0.5f));
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

    ACharacter* PlayerChar = Cast<ACharacter>(OtherActor);
    if (PlayerChar)
    {
        if (auto* MoveComp = PlayerChar->GetCharacterMovement())
        {
            MoveComp->MaxWalkSpeed *= 0.5f;
        }
    }

    UGameplayStatics::ApplyDamage(OtherActor, Dano, GetInstigatorController(), this, UDamageType::StaticClass());
    Destroy();
}