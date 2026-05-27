#pragma once
#include "CoreMinimal.h"
#include "EnemigoTerrestre.h"
#include "RobotFrancotirador.generated.h"

class EstadoRobot;

UCLASS()
class GALAGAMODIFICADOMAC_API ARobotFrancotirador : public AEnemigoTerrestre
{
    GENERATED_BODY()

public:
    ARobotFrancotirador();

    virtual void ActualizarComportamiento() override;
    virtual void Atacar() override;

    float RangoPreparacion;
    float RangoDisparo;

    float TiempoEntreDisparos;
    float TiempoUltimoDisparo;

    void ApuntarAlJugador();

private:
    EstadoRobot* EstadoActual;
};