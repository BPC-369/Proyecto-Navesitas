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

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComando(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow05.StarSparrow05'"));
    MallaCache = MeshComando.Succeeded() ? MeshComando.Object : nullptr;

    RutaExplosion = TEXT("/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Explosion/P_Explosion_Big_B");
    ExplosionScale = 3.0f;
    RutaSonidoExplosion = TEXT("/Game/music/explooosion");
}

void ANaveComando::BeginPlay()
{
    Super::BeginPlay();

    if (ComponenteCombate)
    {
        ComponenteCombate->VidaMaxima = 2500.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

    if (MallaCache && MallaEnemiga)
    {
        MallaEnemiga->SetStaticMesh(MallaCache);
        MallaEnemiga->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
        MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    UWorld* World = GetWorld();
    if (World)
    {
        World->GetTimerManager().SetTimer(TimerAtaque, this, &ANaveComando::Atacar, FrecuenciaAtaque, true);
        World->GetTimerManager().SetTimer(TimerSpawn, this, &ANaveComando::GestionarEscoltas, 2.5f, true);
    }
}

void ANaveComando::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // La barra de vida se gestiona ahora desde el HUD del jugador
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

    for (int32 i = EscoltasActivas.Num() - 1; i >= 0; i--)
    {
        ANaveEnemigoAereo* Escolta = EscoltasActivas[i];
        if (!IsValid(Escolta) || Escolta->IsPendingKill())
        {
            EscoltasActivas.RemoveAt(i);
        }
    }

    if (EscoltasActivas.Num() <= (MAX_ESCOLTAS - NAVES_POR_SPAWN))
    {
        for (int32 j = 0; j < NAVES_POR_SPAWN; j++)
        {
            if (!IsValid(this)) return;

            FVector Offset = (GetActorRightVector() * ((j - 1) * 450.0f)) - (GetActorForwardVector() * 600.0f);
            FVector Pos = GetActorLocation() + Offset;

            FabricaNaves::TipoNave Tipo = FMath::RandBool() ? FabricaNaves::COMUN : FabricaNaves::KAMIKASE;
            ANaveEnemigoAereo* NuevaEscolta = FabricaNaves::CrearNave(Tipo, World, Pos, GetActorRotation());
            if (NuevaEscolta)
            {
                UComponenteCombate* CompEscolta = NuevaEscolta->FindComponentByClass<UComponenteCombate>();
                if (CompEscolta && ComponenteCombate)
                {
                    CompEscolta->Faccion = ComponenteCombate->Faccion;
                }

                UPrimitiveComponent* Collider = NuevaEscolta->FindComponentByClass<UPrimitiveComponent>();
                if (Collider)
                {
                    Collider->IgnoreActorWhenMoving(this, true);
                }

                EscoltasActivas.Add(NuevaEscolta);
            }
        }
    }
}