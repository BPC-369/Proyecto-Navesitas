// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EscenarioBase.h"
#include "EscenarioCiudad.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEscenarioCiudad : public AEscenarioBase
{
	GENERATED_BODY()

public:
	AEscenarioCiudad();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuracion Ciudad")
	int32 CantidadEdificios;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuracion Ciudad")
	int32 CantidadArboles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuracion Ciudad")
	int32 CantidadRocas;

	virtual void GenerarObstaculosProcedurales() override;
};