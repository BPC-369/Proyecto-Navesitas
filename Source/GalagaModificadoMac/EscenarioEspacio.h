// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EscenarioBase.h"
#include "EscenarioEspacio.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEscenarioEspacio : public AEscenarioBase
{
	GENERATED_BODY()

public:
	AEscenarioEspacio();

protected:
	virtual void BeginPlay() override;

private:
	// Tu función de generación procedural se queda aquí
	void GenerarEntornoEspacial();
};