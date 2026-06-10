#include "FabricaRobots.h"
#include "Robot_Lider.h"
#include "Robot_Medico.h"
#include "Robot_Razo.h"
#include "Robot_RZ.h"
#include "Robot_RZ_D.h"
#include "RobotFrancotirador.h"
#include "Engine/World.h"

AEnemigoTerrestre* FabricaRobots::CrearRobot(TipoRobot Tipo, UWorld* World, FVector Ubicacion, FRotator Rotacion)
{
    if (!World) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    switch (Tipo)
    {
    case LIDER:          return World->SpawnActor<ARobot_Lider>(ARobot_Lider::StaticClass(), Ubicacion, Rotacion, Params);
    case MEDICO:         return World->SpawnActor<ARobot_Medico>(ARobot_Medico::StaticClass(), Ubicacion, Rotacion, Params);
    case RAZO:           return World->SpawnActor<ARobot_Razo>(ARobot_Razo::StaticClass(), Ubicacion, Rotacion, Params);
    case RZ:             return World->SpawnActor<ARobot_RZ>(ARobot_RZ::StaticClass(), Ubicacion, Rotacion, Params);
    case RZ_D:           return World->SpawnActor<ARobot_RZ_D>(ARobot_RZ_D::StaticClass(), Ubicacion, Rotacion, Params);
    case FRANCOTIRADOR:  return World->SpawnActor<ARobotFrancotirador>(ARobotFrancotirador::StaticClass(), Ubicacion, Rotacion, Params);
    default:             return nullptr;
    }
}