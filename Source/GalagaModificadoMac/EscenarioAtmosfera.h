// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EscenarioBase.h"
#include "EscenarioAtmosfera.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEscenarioAtmosfera : public AEscenarioBase
{
	GENERATED_BODY()

public:
	AEscenarioAtmosfera();

protected:
	virtual void BeginPlay() override;

public:
	// Mantener estas variables aquí permite que los bucles 'for' funcionen correctamente
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuracion Atmosfera")
	int32 CantidadNubesPiso;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuracion Atmosfera")
	int32 CantidadMontanas;
	virtual void GenerarObstaculosProcedurales() override;
private:
	// Tu lógica procedural se queda en esta clase
	void GenerarPisoDeNubes();
	void GenerarMontanas();
};