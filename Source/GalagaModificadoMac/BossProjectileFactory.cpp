// BossProjectileFactory.cpp
#include "BossProjectileFactory.h"
#include "Engine/World.h"

void BossProjectileFactory::CrearProyectil(UWorld* Mundo, FVector Posicion, FRotator Rotacion, AActor* Disparador, float Escala, float Dano, float Velocidad)
{
    if (!Mundo) return;
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Disparador;
    SpawnParams.Instigator = Disparador->GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(ABossProjectile::StaticClass(), Posicion, Rotacion, SpawnParams);
    if (Proyectil) { Proyectil->ConfigurarProyectil(Escala, Dano, Velocidad); }
}