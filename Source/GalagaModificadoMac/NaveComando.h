#pragma once
#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "NaveComando.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ANaveComando : public ANaveEnemigoAereo
{
    GENERATED_BODY()

public:
    ANaveComando();

protected:
    virtual void BeginPlay() override;
    virtual void Destroyed() override;

public:
    virtual void Atacar() override;

private:
    void GestionarEscoltas();

    FTimerHandle TimerAtaque;
    FTimerHandle TimerSpawn;

    UPROPERTY()
        TArray<ANaveEnemigoAereo*> EscoltasActivas;

    const int32 MAX_ESCOLTAS = 21;
    const int32 NAVES_POR_SPAWN = 3;

    UStaticMesh* MallaCache;
};