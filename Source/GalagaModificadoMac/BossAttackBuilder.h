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

    // Métodos antiguos (conservados)
    void ConstruirMuroRadialAoE(FVector Origen, int32 CantidadDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas = 3, float AnguloBase = 0.0f);
    void ConstruirMuroEspiral(FVector Origen, int32 NumDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas, float AnguloOffset);
    void ConstruirMuroOndaExpansiva(FVector Origen, int32 NumDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas);
    void ConstruirAnilloOndaExpansiva(FVector Origen, int32 CantidadProyectiles, bool ActivarOnda);
    void ConstruirDomoDeMuros(FVector Origen, int32 Anillos, int32 ProyectilesPorAnillo, bool ActivarRalentizacion = false);
    void ConstruirDomoDeProyectiles(FVector Origen, int32 NumProyectiles, bool ActivarRalentizacion = false);

    // NUEVOS MÉTODOS
    void ConstruirMuroFrontal(FVector Origen, FVector DireccionObjetivo, int32 AlturaMuro, float Espaciado, int32 Columnas);
    void ConstruirLluviaTechada(FVector Origen, float Radio, int32 Densidad, float AlturaCaida);

private:
    UWorld* Mundo;
    AActor* Dueno;
    float Dano;
    float Velocidad;
    float Escala;

    void ObtenerDimensionesJefe(float& Radio, float& MitadAltura) const;
};