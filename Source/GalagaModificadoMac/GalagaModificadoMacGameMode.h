#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
// Declaraciones anticipadas para no saturar el compilador
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

	// La función maestra que invocará al ejército
	void GenerarEjercito();

	// --- CONTENEDORES PUROS DE C++ (SIN UPROPERTY) ---
	TArray<ANaveComando*> ListaNavesComando;
	TArray<ATorreta*> ListaTorretas;
	TArray<ARobot_Lider*> ListaRobotsLider;
	TArray<ANaveLider*> ListaNavesLider;
	TArray<ANaveKamikase*> ListaNavesKamikase;
	TArray<ANave_CMN*> ListaNavesCMN;
	TArray<ARobotFrancotirador*> ListaFrancotiradores;

private:
	// --- VARIABLES MODIFICABLES DESDE EL CÓDIGO ---
	int32 CantNaveComando;
	int32 CantTorreta;
	int32 CantRobotLider;
	int32 CantNaveLider;
	int32 CantNaveKamikase;
	int32 CantNaveCMN;
	int32 CantFrancotirador;
};