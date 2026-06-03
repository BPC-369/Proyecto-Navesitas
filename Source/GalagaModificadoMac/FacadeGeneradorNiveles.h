// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FacadeGeneradorNiveles.generated.h"

// DECLARACIONES ANTICIPADAS (Avisa al compilador que estas clases existen sin cargar todo el archivo pesado)
class AEscenarioBase;
class AObstaculoFactory;

// Ficha técnica para controlar la dificultad y la ambientación
USTRUCT(BlueprintType)
struct FConfiguracionNivel
{
	GENERATED_BODY()

	UPROPERTY()
	int32 TipoAmbiente; // 1 = Espacio, 2 = Ciudad, 3 = Atmosfera, 4 = Nave Nodriza

	UPROPERTY()
	int32 CantidadObstaculosA;

	UPROPERTY()
	int32 CantidadObstaculosB;

	UPROPERTY()
	TMap<int32, int32> EnemigosPorGenerar;
};

UCLASS()
class GALAGAMODIFICADOMAC_API UFacadeGeneradorNiveles : public UObject
{
	GENERATED_BODY()

public:
	// Prepara la Fachada y configura la lista de 15 niveles
	void Inicializar(UWorld* WorldContext);

	// El método unificado de campaña
	void CargarNivelPorIndice(int32 IndiceNivel);

	UPROPERTY()
	FConfiguracionNivel ConfiguracionActual;

	// Función para que el GameMode la lea
	FConfiguracionNivel GetConfiguracionActual() const { return ConfiguracionActual; }

private:
	UPROPERTY()
	UWorld* MundoActual;

	// Lista interna con la receta de los 15 niveles
	TArray<FConfiguracionNivel> CampanaNiveles;

	// Punteros declarados de forma segura para Unreal
	UPROPERTY()
	AEscenarioBase* EscenarioActivo;

	UPROPERTY()
	AObstaculoFactory* FabricaActiva;

	void InicializarCampana();
	void DestruirNivelAnterior();
};