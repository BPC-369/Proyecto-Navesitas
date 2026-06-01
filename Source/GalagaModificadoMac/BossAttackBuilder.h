#pragma once
#include "CoreMinimal.h"

class AActor;
class UWorld;

class BossAttackBuilder
{
public:
    BossAttackBuilder(UWorld* EnMundo, AActor* EnDisparador);

    // Métodos para configurar la munición de la pared
    BossAttackBuilder& SetDano(float NuevoDano);
    BossAttackBuilder& SetVelocidad(float NuevaVelocidad);
    BossAttackBuilder& SetEscala(float NuevaEscala);

    // Constructores de Formas Complejas
    void ConstruirPared(FVector Origen, FVector DireccionFrente, int Filas, int Columnas, float Espaciado);
    void ConstruirAbanico(FVector Origen, FVector DireccionFrente, int Cantidad, float AnguloApertura, bool bMulticapa);

private:
    UWorld* Mundo;
    AActor* Disparador;
    float Dano;
    float Velocidad;
    float Escala;
};