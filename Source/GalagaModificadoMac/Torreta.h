#pragma once

#include "CoreMinimal.h"
#include "EnemigoTerrestre.h" // ARREGLO: Quitamos el "GameFramework/" falso
#include "Torreta.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class GALAGAMODIFICADOMAC_API ATorreta : public AEnemigoTerrestre
{
	GENERATED_BODY()

public:
	ATorreta();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ¡UPROPERTY OBLIGATORIOS PARA QUE UNREAL NO BORRE LOS COMPONENTES!
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	UStaticMeshComponent* MallaCanion;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	USphereComponent* RangoDeteccion;

	// Lógica de combate interna expuesta al Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combate Torreta")
	float CadenciaAtaque;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combate Torreta")
	float DistanciaParaAcelerar;

	FTimerHandle TimerDisparo;

	virtual void Atacar() override; // ARREGLO: Agregamos override de la clase padre

	class APawn* JugadorObjetivo;
};