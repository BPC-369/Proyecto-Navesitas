// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FacadeGeneradorNiveles.generated.h"

class AEscenarioBase;
class AObstaculoFactory;

UCLASS()
class GALAGAMODIFICADOMAC_API UFacadeGeneradorNiveles : public UObject
{
	GENERATED_BODY()

public:
	// Función para entregarle el control del mundo a la Fachada
	void Inicializar(UWorld* WorldContext);

	// --- LA FACHADA: Los únicos 4 botones que verá el GameMode ---
	void CargarNivelEspacio();
	void CargarNivelCiudad();
	void CargarNivelAtmosfera();
	void CargarNivelMadreNodriza(); // Listo para cuando lo crees

private:
	UWorld* MundoActual;

	// Referencias para llevar el control y borrar el nivel viejo al cambiar
	UPROPERTY()
	AEscenarioBase* EscenarioActivo;

	UPROPERTY()
	AObstaculoFactory* FabricaActiva;

	// Lógica interna que el GameMode no necesita conocer
	void DestruirNivelAnterior();
};
