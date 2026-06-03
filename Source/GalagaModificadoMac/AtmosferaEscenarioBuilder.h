// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EscenarioBuilder.h"
#include "AtmosferaEscenarioBuilder.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AAtmosferaEscenarioBuilder : public AEscenarioBuilder
{
	GENERATED_BODY()

public:
	virtual void ConstruirDimensiones() override;
	virtual void ConstruirEsteticaCielo() override;
	virtual void ConstruirFisicasSuelo() override;
	virtual void ConstruirFabricaDeObstaculos() override;
	virtual void ConstruirFabricaDeEnemigos() override;
};
