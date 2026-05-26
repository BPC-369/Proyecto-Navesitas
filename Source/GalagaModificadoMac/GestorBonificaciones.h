#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GestorBonificaciones.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AGestorBonificaciones : public AActor
{
	GENERATED_BODY()

public:
	AGestorBonificaciones();

protected:
	virtual void BeginPlay() override;

private:
	// Usamos un Timer puro de C++
	FTimerHandle TimerGeneracion;
	void IntentarGenerarBonificacion();

	// Limites del mapa para que aparezcan (ajusta estos valores al tamaño de tu ciudad/espacio)
	float LimiteX;
	float LimiteY;
};