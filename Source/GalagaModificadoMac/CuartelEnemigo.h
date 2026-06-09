// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CuartelEnemigo.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ACuartelEnemigo : public AActor
{
	GENERATED_BODY()

public:
	ACuartelEnemigo();

protected:
	virtual void BeginPlay() override;

public:
	// Malla visual del cuartel
	UPROPERTY(VisibleAnywhere, Category = "Cuartel | Componentes")
		UStaticMeshComponent* MallaEstructura;

	// ?? Configuración inyectable por el Facade/Builder
	int32 TipoRobotASpawnear; // 9 = Médico, 10 = RZ, 11 = RZ_D
	float TiempoEntreSpawns;

private:
	FTimerHandle TimerProduccion;

	// Función interna que ejecuta el nacimiento del robot
	void ProducirUnidad();
};