#pragma once

#include "CoreMinimal.h"
#include "EnemigoTerrestre.h"
#include "Robot_Razo.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ARobot_Razo : public AEnemigoTerrestre
{
	GENERATED_BODY()
public:
	ARobot_Razo();

	virtual void Tick(float DeltaTime) override;

	// Sobrescribimos el ataque del padre
	virtual void Atacar() override;

protected:
	bool bProtocoloFuriaActivado;

private:
	// La función que se ejecutará después de medio segundo de canalización
	void FinalizarAtaque();

	// Temporizador para contar el medio segundo
	FTimerHandle TimerHandle_Canalizacion;

	// Seguro para saber si el robot ya está en medio de un ataque
	bool bEstaAtacando;
};