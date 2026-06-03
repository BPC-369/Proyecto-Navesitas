#include "BossAttackBuilder.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"      // para USphereComponent
#include "BossEstatico.h"

BossAttackBuilder::BossAttackBuilder(UWorld* InWorld, AActor* InDueno)
    : Mundo(InWorld), Dueno(InDueno), Dano(80.0f), Velocidad(1900.0f), Escala(2.0f) {}

BossAttackBuilder& BossAttackBuilder::SetDano(float InDano) { Dano = InDano; return *this; }
BossAttackBuilder& BossAttackBuilder::SetVelocidad(float InVelocidad) { Velocidad = InVelocidad; return *this; }
BossAttackBuilder& BossAttackBuilder::SetEscala(float InEscala) { Escala = InEscala; return *this; }

void BossAttackBuilder::ConstruirMuroRadialAoE(FVector Origen, int32 CantidadDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas, float AnguloBase)
{
    if (!Mundo || CantidadDirecciones <= 0 || AlturaMuro <= 0 || Columnas <= 0) return;

    float RadioSeguro = 300.0f;
    if (ABossEstatico* Jefe = Cast<ABossEstatico>(Dueno))
    {
        if (UCapsuleComponent* Capsula = Cast<UCapsuleComponent>(Jefe->GetRootComponent()))
        {
            RadioSeguro = Capsula->GetScaledCapsuleRadius() + 100.0f;
        }
    }

    float AnguloPaso = 360.0f / CantidadDirecciones;

    for (int32 d = 0; d < CantidadDirecciones; ++d)
    {
        float AnguloActual = AnguloBase + (d * AnguloPaso);
        FVector Direccion = FVector(1, 0, 0).RotateAngleAxis(AnguloActual, FVector::UpVector);
        FVector OrigenLinea = Origen + (Direccion * RadioSeguro);
        FVector Perpendicular = FVector::CrossProduct(Direccion, FVector::UpVector).GetSafeNormal();

        for (int32 h = 0; h < AlturaMuro; ++h)
        {
            FVector BaseVertical = OrigenLinea + (FVector::UpVector * h * Espaciado);

            for (int32 c = 0; c < Columnas; ++c)
            {
                float OffsetLateral = (c - (Columnas - 1) * 0.5f) * Espaciado;
                FVector PosicionSpawn = BaseVertical + (Perpendicular * OffsetLateral);

                FActorSpawnParameters Params;
                Params.Instigator = Cast<APawn>(Dueno);
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
                    ABossProjectile::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, Params);
                if (Proyectil)
                {
                    Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
                    Proyectil->SetDireccion(Direccion);
                    // Usar el colisionador esfera en lugar de MallaProyectil
                    if (Proyectil->Colisionador)
                    {
                        Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
                    }
                }
            }
        }
    }
}

void BossAttackBuilder::ConstruirAnilloOndaExpansiva(FVector Origen, int32 CantidadProyectiles, bool ActivarOnda)
{
    if (!Mundo || CantidadProyectiles <= 0) return;

    float RadioSeguro = 300.0f;
    if (ABossEstatico* Jefe = Cast<ABossEstatico>(Dueno))
    {
        if (UCapsuleComponent* Capsula = Cast<UCapsuleComponent>(Jefe->GetRootComponent()))
        {
            RadioSeguro = Capsula->GetScaledCapsuleRadius() + 100.0f;
        }
    }

    float AnguloPaso = 360.0f / CantidadProyectiles;

    for (int32 i = 0; i < CantidadProyectiles; ++i)
    {
        float AnguloActual = i * AnguloPaso;
        FVector DirRotada = FVector(1, 0, 0).RotateAngleAxis(AnguloActual, FVector::UpVector);
        FVector PosicionSpawn = Origen + (DirRotada * RadioSeguro);

        FActorSpawnParameters Params;
        Params.Instigator = Cast<APawn>(Dueno);
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
            ABossProjectile::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, Params);
        if (Proyectil)
        {
            Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
            Proyectil->SetDireccion(DirRotada);
            if (Proyectil->Colisionador)
            {
                Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
            }

            if (ActivarOnda)
            {
                Proyectil->HabilitarEfectoOnda(12.0f, 650.0f);
            }
        }
    }
}