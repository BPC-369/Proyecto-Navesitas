// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscenarioBuilder.h"
#include "EscenarioDirector.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEscenarioDirector : public AActor
{
	GENERATED_BODY()

public:
	AEscenarioDirector();

	AEscenarioBase* ConstruirEscenario(AEscenarioBuilder* BuilderElegido, TSubclassOf<AEscenarioBase> ClaseEscenario);
};