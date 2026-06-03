// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscenarioBase.h"
#include "EscenarioBuilder.generated.h"

UCLASS(Abstract)
class GALAGAMODIFICADOMAC_API AEscenarioBuilder : public AActor
{
	GENERATED_BODY()

public:
	AEscenarioBuilder();

protected:
	// El puntero que guardará el escenario físico mientras lo equipamos
	UPROPERTY()
	AEscenarioBase* EscenarioEnConstruccion;

public:
	// Paso Inicial: Spawnea el cascarón del escenario vacío en el mapa
	void InstanciarEscenario(TSubclassOf<AEscenarioBase> ClaseEscenario)
	{
		if (GetWorld() && ClaseEscenario)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Nace el escenario en la coordenada (0,0,0)
			EscenarioEnConstruccion = GetWorld()->SpawnActor<AEscenarioBase>(
				ClaseEscenario,
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				SpawnParams
			);
		}
	}

	// LA RECETA: Métodos virtuales que tus builders concretos van a rellenar
	virtual void ConstruirDimensiones() PURE_VIRTUAL(AEscenarioBuilder::ConstruirDimensiones, );
	virtual void ConstruirEsteticaCielo() PURE_VIRTUAL(AEscenarioBuilder::ConstruirEsteticaCielo, );
	virtual void ConstruirFisicasSuelo() PURE_VIRTUAL(AEscenarioBuilder::ConstruirFisicasSuelo, );
	virtual void ConstruirFabricaDeObstaculos() PURE_VIRTUAL(AEscenarioBuilder::ConstruirFabricaDeObstaculos, );
	virtual void ConstruirFabricaDeEnemigos() PURE_VIRTUAL(AEscenarioBuilder::ConstruirFabricaDeEnemigos, );

	// Entrega el producto terminado
	AEscenarioBase* GetEscenario() const { return EscenarioEnConstruccion; }
};