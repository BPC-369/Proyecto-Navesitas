#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GalagaModificadoMacGameMode.generated.h"

// DECLARACIONES ANTICIPADAS (Obligatorias para que no falle la línea 12 ni los TArray)
class UFacadeGeneradorNiveles;
class ANaveComando;
class ATorreta;
class ARobot_Lider;
class ANaveLider;
class ANaveKamikase;
class ANave_CMN;
class ARobotFrancotirador;

UCLASS()
class GALAGAMODIFICADOMAC_API AGalagaModificadoMacGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGalagaModificadoMacGameMode();

protected:
	virtual void BeginPlay() override;

public:
	int32 NivelAIniciar;

	UPROPERTY()
	UFacadeGeneradorNiveles* GerenteDeNiveles;

	// ?? VARIABLES DE CONTEO (Sincronizadas exactamente con lo que inyecta tu Fachada)
	int32 CantNaveComando;
	int32 CantTorreta;
	int32 CantRobotLider;
	int32 CantNaveLider;
	int32 CantNaveKamikase;
	int32 CantNaveCMN;
	int32 CantFrancotirador; // Sincronizado con el .cpp de tus compañeros

	int32 AmbienteActual;

	// ?? LOS ARREGLOS QUE TE FALTABAN (Para guardar los enemigos creados)
	UPROPERTY()
	TArray<ANaveComando*> ListaNavesComando;

	UPROPERTY()
	TArray<ATorreta*> ListaTorretas;

	UPROPERTY()
	TArray<ARobot_Lider*> ListaRobotsLider;

	UPROPERTY()
	TArray<ANaveLider*> ListaNavesLider;

	UPROPERTY()
	TArray<ANaveKamikase*> ListaNavesKamikase;

	UPROPERTY()
	TArray<ANave_CMN*> ListaNavesCMN;

	UPROPERTY()
	TArray<ARobotFrancotirador*> ListaFrancotiradores;

	void GenerarEjercito();
};