#pragma once

#include "CoreMinimal.h"
#include "EnemigoTerrestre.h" 
#include "Torreta.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class GALAGAMODIFICADOMAC_API ATorreta : public AEnemigoTerrestre
{
	GENERATED_BODY()

public:
	ATorreta();
	virtual void Tick(float DeltaTime) override;
	virtual void Atacar() override;

protected:
	virtual void BeginPlay() override;

public:
	UStaticMeshComponent* MallaCanion;
	USphereComponent* RangoDeteccion;
	float CadenciaAtaque;
	float DistanciaParaAcelerar;
	FTimerHandle TimerDisparo;

	// Añadir UPROPERTY lo inicializa en nullptr automáticamente y lo protege en memoria
	UPROPERTY()
	class APawn* JugadorObjetivo;
};