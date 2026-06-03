#pragma once

#include "CoreMinimal.h"
#include "BossProjectile.h"

class UWorld;
class AActor;

class GALAGAMODIFICADOMAC_API BossAttackBuilder
{
public:
    BossAttackBuilder(UWorld* InWorld, AActor* InDueno);

    BossAttackBuilder& SetDano(float InDano);
    BossAttackBuilder& SetVelocidad(float InVelocidad);
    BossAttackBuilder& SetEscala(float InEscala);

    // Ahora acepta un ángulo base para rotar el patrón (en grados)
    void ConstruirMuroRadialAoE(FVector Origen, int32 CantidadDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas = 3, float AnguloBase = 0.0f);
    void ConstruirAnilloOndaExpansiva(FVector Origen, int32 CantidadProyectiles, bool ActivarOnda);

private:
    UWorld* Mundo;
    AActor* Dueno;
    float Dano;
    float Velocidad;
    float Escala;
};