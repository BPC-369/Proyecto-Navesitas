// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "EscenarioBase.h" 
#include "Components/StaticMeshComponent.h"
#include "EscenarioCiudad.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEscenarioCiudad : public AEscenarioBase
{
	GENERATED_BODY()

public:
	AEscenarioCiudad();

protected:

	virtual void BeginPlay() override;

private:
	
	TArray<UStaticMeshComponent*> ContenedorEdificiosDisponibles;

	int32 CantidadEdificios;

	void GenerarObstaculosProcedurales();
};