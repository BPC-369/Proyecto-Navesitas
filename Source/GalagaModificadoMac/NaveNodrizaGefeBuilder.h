// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "NaveNodrizaEscenarioBuilder.h" 
#include "NaveNodrizaGefeBuilder.generated.h" 

UCLASS()
class GALAGAMODIFICADOMAC_API ANaveNodrizaGefeBuilder : public ANaveNodrizaEscenarioBuilder
{
	GENERATED_BODY()

public:
	// Sobrescribimos únicamente la estética del cielo para cambiar la malla/material
	virtual void ConstruirEsteticaCielo() override;
};