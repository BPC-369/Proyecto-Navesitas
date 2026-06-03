#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

// Declaraciones anticipadas
class ANaveComando;
class ATorreta;
class ARobot_Lider;
class ANaveLider;
class ANaveKamikase;
class ANave_CMN;
class ARobotFrancotirador;

#include "GalagaModificadoMacGameMode.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AGalagaModificadoMacGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGalagaModificadoMacGameMode();

	virtual void BeginPlay() override;

	void GenerarEjercito(TMap<int32, int32> EnemigosDelNivel);

	// --- CONTENEDORES PUROS DE C++ ---
	TArray<ANaveComando*> ListaNavesComando;
	TArray<ATorreta*> ListaTorretas;
	TArray<ARobot_Lider*> ListaRobotsLider;
	TArray<ANaveLider*> ListaNavesLider;
	TArray<ANaveKamikase*> ListaNavesKamikase;
	TArray<ANave_CMN*> ListaNavesCMN;
	TArray<ARobotFrancotirador*> ListaFrancotiradores;

	UPROPERTY()
	class UFacadeGeneradorNiveles* GerenteDeNiveles;

	// Variable C++ pura para elegir nivel
	int32 NivelAIniciar;
};