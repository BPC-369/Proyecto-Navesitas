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

    // Muro original (sin uso actual, se conserva)
    void ConstruirMuroRadialAoE(FVector Origen, int32 CantidadDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas = 3, float AnguloBase = 0.0f);

    // Muro en espiral (paredes verticales giratorias)
    void ConstruirMuroEspiral(FVector Origen, int32 NumDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas, float AnguloOffset);

    // NUEVO: Muro de onda expansiva (muro circular que sale hacia afuera con efecto de onda)
    void ConstruirMuroOndaExpansiva(FVector Origen, int32 NumDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas);

    // Anillo de onda expansiva (un solo proyectil por dirección, se mantiene para otro uso)
    void ConstruirAnilloOndaExpansiva(FVector Origen, int32 CantidadProyectiles, bool ActivarOnda);

    // Domo de muros (semiesfera superior)
    void ConstruirDomoDeMuros(FVector Origen, int32 Anillos, int32 ProyectilesPorAnillo, bool ActivarRalentizacion = false);

    // Nube aleatoria de proyectiles
    void ConstruirDomoDeProyectiles(FVector Origen, int32 NumProyectiles, bool ActivarRalentizacion = false);

private:
    UWorld* Mundo;
    AActor* Dueno;
    float Dano;
    float Velocidad;
    float Escala;

    void ObtenerDimensionesJefe(float& Radio, float& MitadAltura) const;
};