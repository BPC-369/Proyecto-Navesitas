#pragma once

#include "CoreMinimal.h"

class AEnemigoTerrestre;

class GALAGAMODIFICADOMAC_API FabricaRobots
{
public:
    enum TipoRobot
    {
        LIDER,
        MEDICO,
        RAZO,
        RZ,
        RZ_D,
        FRANCOTIRADOR
    };

    static AEnemigoTerrestre* CrearRobot(TipoRobot Tipo, UWorld* World, FVector Ubicacion, FRotator Rotacion);
};