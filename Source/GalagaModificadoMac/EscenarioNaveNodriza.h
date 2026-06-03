// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EscenarioBase.h"
#include "EscenarioNaveNodriza.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEscenarioNaveNodriza : public AEscenarioBase
{
	GENERATED_BODY()

public:
	AEscenarioNaveNodriza();

protected:
	virtual void BeginPlay() override;

public:
	// Variables independientes controlables desde el Builder
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuracion Nodriza")
	int32 CantidadPasillos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuracion Nodriza")
	int32 CantidadTorretas;

	virtual void GenerarObstaculosProcedurales() override;
};