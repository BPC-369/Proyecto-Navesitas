// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FacadeGeneradorNiveles.generated.h"

class AEscenarioBase;
class AObstaculoFactory;
class AEscenarioBuilder;

USTRUCT(BlueprintType)
struct FConfiguracionNivel
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TipoAmbiente; // 1 = Espacio, 2 = Ciudad, 3 = Atmosfera, 4 = Nave Nodriza

	// ?? CONTROL DE OBSTÁCULOS PROCEDURALES (A = Malla principal, B = Malla secundaria)
	UPROPERTY()
	int32 CantidadObstaculosA;
	UPROPERTY()
	int32 CantidadObstaculosB;


	UPROPERTY()
	int32 CantNaveComando;
	UPROPERTY()
	int32 CantTorreta;
	UPROPERTY()
	int32 CantRobotLider;
	UPROPERTY()
	int32 CantNaveLider;
	UPROPERTY()
	int32 CantNaveKamikase;
	UPROPERTY()
	int32 CantNaveCMN;
	UPROPERTY()
	int32 CantRobotFrancotirador;
};

UCLASS()
class GALAGAMODIFICADOMAC_API UFacadeGeneradorNiveles : public UObject
{
	GENERATED_BODY()

public:
	void Inicializar(UWorld* WorldContext);
	void CargarNivelPorIndice(int32 IndiceNivel);

private:
	UPROPERTY()
	UWorld* MundoActual;

	TArray<FConfiguracionNivel> CampanaNiveles;

	UPROPERTY()
	AEscenarioBase* EscenarioActivo;

	UPROPERTY()
	AObstaculoFactory* FabricaActiva;

	void InicializarCampana();
	void DestruirNivelAnterior();
};