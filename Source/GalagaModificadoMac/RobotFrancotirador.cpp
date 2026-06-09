#include "RobotFrancotirador.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "ComponenteCombate.h"
#include "EstadosRobot.h"

ARobotFrancotirador::ARobotFrancotirador()
{
    PrimaryActorTick.bCanEverTick = true;

    RangoDeteccion = 3000.0f;
    RangoPreparacion = 2000.0f;
    RangoDisparo = 1500.0f;
    DanioBase = 45.0f;
    VelocidadMovimiento = 200.0f;

    TiempoEntreDisparos = 2.5f;
    TiempoUltimoDisparo = 0.0f;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaFrancotirador(TEXT("SkeletalMesh'/Game/Geometry/francotirador/mallaFrancotirador.mallaFrancotirador'"));
    if (MallaFrancotirador.Succeeded() && MallaEnemiga != nullptr)
    {
        MallaEnemiga->SetStaticMesh(MallaFrancotirador.Object);
        MallaEnemiga->SetWorldScale3D(FVector(0.5f, 0.5f, 1.8f));
    }

    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 30.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
    }

    EstadoActual = new EstadoDeteccion();
}

void ARobotFrancotirador::ActualizarComportamiento()
{
    float Distancia = CalcularDistanciaAlJugador();
    if (Distancia < 0.0f || Distancia > RangoDeteccion) return;

    EstadoRobot* NuevoEstado = EstadoActual->Ejecutar(this);
    if (NuevoEstado != nullptr)
    {
        delete EstadoActual;
        EstadoActual = NuevoEstado;
    }
}

void ARobotFrancotirador::ApuntarAlJugador()
{
    APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Jugador)
    {
        FVector DireccionHaciaJugador = Jugador->GetActorLocation() - GetActorLocation();
        DireccionHaciaJugador.Z = 0.0f;
        SetActorRotation(DireccionHaciaJugador.Rotation());
    }
}

void ARobotFrancotirador::Atacar()
{
    APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Jugador) return;

    FVector Inicio = GetActorLocation();
    FVector Fin = Jugador->GetActorLocation();

    FHitResult ResultadoImpacto;
    FCollisionQueryParams ParametrosColision;
    ParametrosColision.AddIgnoredActor(this);

    bool bImpacto = GetWorld()->LineTraceSingleByChannel(ResultadoImpacto, Inicio, Fin, ECC_Visibility, ParametrosColision);
    DrawDebugLine(GetWorld(), Inicio, Fin, FColor::Red, false, 0.5f, 0, 3.0f);

    if (bImpacto)
    {
        AActor* ActorGolpeado = ResultadoImpacto.GetActor();
        if (ActorGolpeado && ActorGolpeado == Jugador)
        {
            UGameplayStatics::ApplyDamage(ActorGolpeado, DanioBase, GetController(), this, UDamageType::StaticClass());
            UE_LOG(LogTemp, Warning, TEXT("¡Francotirador Acertó! Daño infligido: %f"), DanioBase);
        }
    }
}