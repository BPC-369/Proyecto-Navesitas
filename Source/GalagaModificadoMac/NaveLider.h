#pragma once

#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "NaveLider.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ANaveLider : public ANaveEnemigoAereo
{
    GENERATED_BODY()

public:
    ANaveLider();

    int32 ContadorDisparos;
    float EnergiaEscudo;
    float VidaLider;

    virtual void Atacar() override;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    FTimerHandle TimerAtaque;

    // Malla que se asignara al proyectil enemigo tras el spawn
    UStaticMesh* MallaProyectilEnemigo;
};