#pragma once

#include "CoreMinimal.h"
#include "EnemigoTerrestre.h" 
#include "Torreta.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent; // ?? Declaración anticipada limpia

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
		USkeletalMeshComponent* MeshTorretaAnimada;

	UStaticMeshComponent* MallaCanion;
	USphereComponent* RangoDeteccion;
	float CadenciaAtaque;
	float DistanciaParaAcelerar;
	FTimerHandle TimerDisparo;

	UPROPERTY()
		class APawn* JugadorObjetivo;
};