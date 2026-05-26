#pragma once
#include "CoreMinimal.h"
#include "EnemigoTerrestre.h"
#include "RobotFrancotirador.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ARobotFrancotirador : public AEnemigoTerrestre
{
	GENERATED_BODY()

public:
	ARobotFrancotirador();

	virtual void ActualizarComportamiento() override;
	virtual void Atacar() override;

protected:
	// Los 3 rangos solicitados
	float RangoPreparacion;
	float RangoDisparo;

	float TiempoEntreDisparos;
	float TiempoUltimoDisparo;

	// Función auxiliar para que el robot gire hacia el jugador mientras se prepara
	void ApuntarAlJugador();
};
