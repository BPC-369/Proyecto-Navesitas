#include "Nave_CMN.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GalagaModificadoMacProjectile.h"
#include "ComponenteCombate.h"

ANave_CMN::ANave_CMN()
{
    DanioAtaque = 10.0f;
    Velocidad = 300.0f;
    FrecuenciaAtaque = 0.5f;

    bTieneEscudo = false;
    bEsElite = false;

    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 50.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCono(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow06.StarSparrow06'"));
    if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
    {
        MallaEnemiga->SetStaticMesh(FormaCono.Object);
        MallaEnemiga->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
        MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    // Carga la malla que se usara en los proyectiles enemigos
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaEnemigaAsset(TEXT("/Game/TwinStick/Meshes/BP_EnemyProjectile"));
    MallaProyectilEnemigo = MallaEnemigaAsset.Succeeded() ? MallaEnemigaAsset.Object : nullptr;
}

void ANave_CMN::BeginPlay()
{
    Super::BeginPlay();
    GetWorld()->GetTimerManager().SetTimer(TemporizadorAtaque, this, &ANave_CMN::Atacar, FrecuenciaAtaque, true);
}

void ANave_CMN::ConvertirAElite()
{
    if (!bEsElite)
    {
        bEsElite = true;
        Velocidad = 900.0f;
        FrecuenciaAtaque = 0.25f;

        GetWorld()->GetTimerManager().ClearTimer(TemporizadorAtaque);
        GetWorld()->GetTimerManager().SetTimer(TemporizadorAtaque, this, &ANave_CMN::Atacar, FrecuenciaAtaque, true);
    }
}

void ANave_CMN::Atacar()
{
    UWorld* const World = GetWorld();
    if (World != nullptr)
    {
        const FRotator RotacionDisparo = GetActorRotation();
        float DistanciaFrontal = 120.0f;
        float SeparacionLateral = 80.0f;

        FVector PosicionBase = GetActorLocation();
        FVector DireccionFrontal = GetActorForwardVector();
        FVector DireccionDerecha = GetActorRightVector();

        FVector PosicionDisparoDerecha = PosicionBase + (DireccionFrontal * DistanciaFrontal) + (DireccionDerecha * SeparacionLateral);
        FVector PosicionDisparoIzquierda = PosicionBase + (DireccionFrontal * DistanciaFrontal) - (DireccionDerecha * SeparacionLateral);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // Proyectil derecho
        AGalagaModificadoMacProjectile* ProyectilDerecho = World->SpawnActor<AGalagaModificadoMacProjectile>(
            AGalagaModificadoMacProjectile::StaticClass(), PosicionDisparoDerecha, RotacionDisparo, SpawnParams);
        if (ProyectilDerecho)
        {
            // Ignorar colision con la nave
            UPrimitiveComponent* ColliderDerecho = ProyectilDerecho->FindComponentByClass<UPrimitiveComponent>();
            if (ColliderDerecho)
            {
                ColliderDerecho->IgnoreActorWhenMoving(this, true);
            }

            // Aplicar la malla enemiga
            if (MallaProyectilEnemigo)
            {
                UStaticMeshComponent* MeshComp = ProyectilDerecho->FindComponentByClass<UStaticMeshComponent>();
                if (MeshComp)
                {
                    MeshComp->SetStaticMesh(MallaProyectilEnemigo);
                }
            }
        }

        // Proyectil izquierdo
        AGalagaModificadoMacProjectile* ProyectilIzquierdo = World->SpawnActor<AGalagaModificadoMacProjectile>(
            AGalagaModificadoMacProjectile::StaticClass(), PosicionDisparoIzquierda, RotacionDisparo, SpawnParams);
        if (ProyectilIzquierdo)
        {
            UPrimitiveComponent* ColliderIzquierdo = ProyectilIzquierdo->FindComponentByClass<UPrimitiveComponent>();
            if (ColliderIzquierdo)
            {
                ColliderIzquierdo->IgnoreActorWhenMoving(this, true);
            }

            if (MallaProyectilEnemigo)
            {
                UStaticMeshComponent* MeshComp = ProyectilIzquierdo->FindComponentByClass<UStaticMeshComponent>();
                if (MeshComp)
                {
                    MeshComp->SetStaticMesh(MallaProyectilEnemigo);
                }
            }
        }
    }
}