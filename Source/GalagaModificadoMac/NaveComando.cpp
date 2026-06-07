#include "NaveComando.h"
#include "FabricaNaves.h"
#include "GalagaModificadoMacProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "ProyectilJefe.h"
#include "ComponenteCombate.h"
#include "Components/PrimitiveComponent.h"

ANaveComando::ANaveComando()
{
    Velocidad = 10.0f;
    FrecuenciaAtaque = 2.0f;

    // Cargar la malla en el constructor (único lugar permitido)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComando(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow05.StarSparrow05'"));
    MallaCache = MeshComando.Succeeded() ? MeshComando.Object : nullptr;
}

void ANaveComando::BeginPlay()
{
    Super::BeginPlay();

    // Configurar vida y facción
    if (ComponenteCombate)
    {
        ComponenteCombate->VidaMaxima = 2500.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

    // Aplicar la malla cacheada
    if (MallaCache && MallaEnemiga)
    {
        MallaEnemiga->SetStaticMesh(MallaCache);
        MallaEnemiga->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
        MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    // Iniciar temporizadores
    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().SetTimer(TimerAtaque, this, &ANaveComando::Atacar, FrecuenciaAtaque, true);
        World->GetTimerManager().SetTimer(TimerSpawn, this, &ANaveComando::GestionarEscoltas, 2.5f, true);
    }
}

void ANaveComando::Destroyed()
{
    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().ClearTimer(TimerAtaque);
        World->GetTimerManager().ClearTimer(TimerSpawn);
    }
    Super::Destroyed();
}

void ANaveComando::Atacar()
{
    if (!IsValid(this) || !GetWorld()) return;

    UWorld* const World = GetWorld();
    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector Frente = GetActorLocation() + (GetActorForwardVector() * 400.0f);
    World->SpawnActor<AProyectilJefe>(Frente, GetActorRotation(), Params);

    FVector Derecha = GetActorLocation() + (GetActorRightVector() * 450.0f);
    FVector Izquierda = GetActorLocation() - (GetActorRightVector() * 450.0f);

    World->SpawnActor<AProyectilJefe>(Derecha, GetActorRightVector().Rotation(), Params);
    World->SpawnActor<AProyectilJefe>(Izquierda, (-GetActorRightVector()).Rotation(), Params);
}

void ANaveComando::GestionarEscoltas()
{
    if (!IsValid(this) || !GetWorld()) return;

    UWorld* const World = GetWorld();

    // Eliminar escoltas inválidas
    for (int32 i = EscoltasActivas.Num() - 1; i >= 0; i--)
    {
        ANaveEnemigoAereo* Escolta = EscoltasActivas[i];
        if (!IsValid(Escolta) || Escolta->IsPendingKill())
        {
            EscoltasActivas.RemoveAt(i);
        }
    }

    // Generar nuevas si hay hueco
    if (EscoltasActivas.Num() <= (MAX_ESCOLTAS - NAVES_POR_SPAWN))
    {
        for (int32 j = 0; j < NAVES_POR_SPAWN; j++)
        {
            if (!IsValid(this)) return;

            FVector Offset = (GetActorRightVector() * ((j - 1) * 450.0f)) - (GetActorForwardVector() * 600.0f);
            FVector Pos = GetActorLocation() + Offset;

            FabricaNaves::TipoNave TipoASpawnear = FMath::RandBool() ? FabricaNaves::COMUN : FabricaNaves::KAMIKASE;
            ANaveEnemigoAereo* NuevaEscolta = FabricaNaves::CrearNave(TipoASpawnear, World, Pos, GetActorRotation());
            if (NuevaEscolta)
            {
                UComponenteCombate* CompEscolta = NuevaEscolta->FindComponentByClass<UComponenteCombate>();
                if (CompEscolta && ComponenteCombate)
                {
                    CompEscolta->Faccion = ComponenteCombate->Faccion;
                }

                UPrimitiveComponent* ColliderEscolta = NuevaEscolta->FindComponentByClass<UPrimitiveComponent>();
                if (ColliderEscolta)
                {
                    ColliderEscolta->IgnoreActorWhenMoving(this, true);
                }

                EscoltasActivas.Add(NuevaEscolta);
            }
        }
    }
}