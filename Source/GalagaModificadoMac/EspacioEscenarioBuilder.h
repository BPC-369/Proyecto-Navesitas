// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EscenarioBuilder.h"
#include "EspacioEscenarioBuilder.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEspacioEscenarioBuilder : public AEscenarioBuilder
{
	GENERATED_BODY()

public:
	// Un solo constructor declarado
	AEspacioEscenarioBuilder();

	// Métodos obligatorios
	virtual void ConstruirDimensiones() override;
	virtual void ConstruirEsteticaCielo() override;
	virtual void ConstruirFisicasSuelo() override;
	virtual void ConstruirFabricaDeObstaculos() override;
	virtual void ConstruirFabricaDeEnemigos() override;

private:

	UPROPERTY()
	UMaterialInterface* MaterialEspacioGuardado;
};