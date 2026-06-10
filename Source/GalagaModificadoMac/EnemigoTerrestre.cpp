#include "EnemigoTerrestre.h"
#include "Kismet/GameplayStatics.h"
#include "ComponenteCombate.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemigoTerrestre::AEnemigoTerrestre()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    MallaEnemiga = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEnemiga"));
    MallaEnemiga->SetupAttachment(GetCapsuleComponent());
    MallaEnemiga->SetNotifyRigidBodyCollision(true);

    ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));

    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 50.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->Faccion = FName("Enemigo");
    }
    VelocidadMovimiento = 300.0f;
    RangoDeteccion = 500.0f;
    DanioBase = 10.0f;
    RangoAtaque = 150.0f;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
    GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
    GetCharacterMovement()->GravityScale = 2.0f;
}

void AEnemigoTerrestre::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    ActualizarComportamiento();
}

void AEnemigoTerrestre::Mover()
{
    APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Jugador)
    {
        FVector MiPosicion = GetActorLocation();
        FVector PosicionJugador = Jugador->GetActorLocation();

        FVector DireccionHaciaJugador = PosicionJugador - MiPosicion;
        DireccionHaciaJugador.Z = 0.0f;
        DireccionHaciaJugador.Normalize();

        AddMovementInput(DireccionHaciaJugador, 1.0f);
    }
}

float AEnemigoTerrestre::CalcularDistanciaAlJugador()
{
    APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Jugador)
    {
        return FVector::Distance(GetActorLocation(), Jugador->GetActorLocation());
    }
    return -1.0f;
}

bool AEnemigoTerrestre::DetectarObjetivo()
{
    float Distancia = CalcularDistanciaAlJugador();
    return (Distancia > 0.0f && Distancia <= RangoDeteccion);
}

void AEnemigoTerrestre::ActualizarComportamiento()
{
    bool bJugadorEnLaMira = DetectarObjetivo();

    if (bJugadorEnLaMira)
    {
        Mover();

        if (bEstaAtacando) return;

        float DistanciaActual = CalcularDistanciaAlJugador();
        if (DistanciaActual <= RangoAtaque)
        {
            if (!bEstaSiendoCurado)
            {
                GetCharacterMovement()->MaxWalkSpeed = 100.0f;
            }

            Atacar();
        }
        else
        {
            if (!bEstaSiendoCurado)
            {
                GetCharacterMovement()->MaxWalkSpeed = 400.0f;
            }
        }
    }
    else
    {
        if (bEstaAtacando) return;

        if (!bEstaSiendoCurado)
        {
            GetCharacterMovement()->MaxWalkSpeed = 150.0f;
        }

        Merodear();
    }
}

void AEnemigoTerrestre::Atacar()
{

}

float AEnemigoTerrestre::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ComponenteCombate != nullptr)
    {
        DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);
    }

    return DanioReal;
}

void AEnemigoTerrestre::Merodear()
{
    TiempoParaCambiarRuta -= GetWorld()->GetDeltaSeconds();

    if (TiempoParaCambiarRuta <= 0.0f)
    {
        DireccionMerodeo = FVector(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), 0.0f).GetSafeNormal();
        TiempoParaCambiarRuta = FMath::RandRange(2.0f, 5.0f);
    }

    AddMovementInput(DireccionMerodeo, 1.0f);
}