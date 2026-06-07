#include "NaveLider.h"
#include "BalaEspecial.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "ComponenteCombate.h"
#include "GalagaModificadoMacProjectile.h"

ANaveLider::ANaveLider()
{
    PrimaryActorTick.bCanEverTick = true;
    ContadorDisparos = 0;

    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 50.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->EscudoMaximo = 0.0f;
        ComponenteCombate->EscudoActual = ComponenteCombate->EscudoMaximo;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow01.StarSparrow01'"));
    if (MeshAsset.Succeeded())
    {
        MallaEnemiga->SetStaticMesh(MeshAsset.Object);
        MallaEnemiga->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
        MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    // Carga la malla que se usara en los proyectiles enemigos
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaEnemigaAsset(TEXT("/Game/TwinStick/Meshes/BP_EnemyProjectile"));
    MallaProyectilEnemigo = MallaEnemigaAsset.Succeeded() ? MallaEnemigaAsset.Object : nullptr;
}

void ANaveLider::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(TimerAtaque, this, &ANaveLider::Atacar, FrecuenciaAtaque, true);
}

void ANaveLider::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ANaveLider::Atacar()
{
    UWorld* const World = GetWorld();
    if (World != nullptr)
    {
        const FRotator RotacionDisparo = GetActorRotation();
        const FVector PosicionDisparo = GetActorLocation() + (GetActorForwardVector() * 180.0f);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // Spawn del proyectil usando la clase base
        AGalagaModificadoMacProjectile* ProyectilLider = World->SpawnActor<AGalagaModificadoMacProjectile>(
            AGalagaModificadoMacProjectile::StaticClass(), PosicionDisparo, RotacionDisparo, SpawnParams);

        if (ProyectilLider)
        {
            // Ignorar colision con la nave
            UPrimitiveComponent* ColliderProyectil = ProyectilLider->FindComponentByClass<UPrimitiveComponent>();
            if (ColliderProyectil)
            {
                ColliderProyectil->IgnoreActorWhenMoving(this, true);
            }

            // Aplicar la malla enemiga
            if (MallaProyectilEnemigo)
            {
                UStaticMeshComponent* MeshComp = ProyectilLider->FindComponentByClass<UStaticMeshComponent>();
                if (MeshComp)
                {
                    MeshComp->SetStaticMesh(MallaProyectilEnemigo);
                }
            }
        }
    }
}