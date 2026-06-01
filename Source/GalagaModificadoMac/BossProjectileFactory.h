// BossProjectileFactory.h
#pragma once
#include "CoreMinimal.h"
#include "BossProjectile.h"

class BossProjectileFactory
{
public:
    static void CrearProyectil(class UWorld* Mundo, FVector Posicion, FRotator Rotacion, AActor* Disparador, float Escala, float Dano, float Velocidad);
};
