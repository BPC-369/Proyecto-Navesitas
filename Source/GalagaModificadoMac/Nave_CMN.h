#pragma once

#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "Nave_CMN.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ANave_CMN : public ANaveEnemigoAereo
{
    GENERATED_BODY()
public:
    ANave_CMN();

    void ConvertirAElite();

protected:
    virtual void BeginPlay() override;
    virtual void Atacar() override;

private:
    bool bEsElite;
    FTimerHandle TemporizadorAtaque;

    // Malla que se asignara al proyectil enemigo tras el spawn
    UStaticMesh* MallaProyectilEnemigo;
};