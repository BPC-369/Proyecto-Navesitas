#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GalagaModificadoMacGameMode.generated.h"

class ANaveComando;
class ATorreta;
class ARobot_Lider;
class ANaveLider;
class ANaveKamikase;
class ANave_CMN;
class ARobotFrancotirador;
class ABossEstatico;

UCLASS()
class GALAGAMODIFICADOMAC_API AGalagaModificadoMacGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGalagaModificadoMacGameMode();

	virtual void BeginPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void GenerarEjercito(TMap<int32, int32> EnemigosDelNivel);

	TArray<ANaveComando*> ListaNavesComando;
	TArray<ATorreta*> ListaTorretas;
	TArray<ARobot_Lider*> ListaRobotsLider;
	TArray<ANaveLider*> ListaNavesLider;
	TArray<ANaveKamikase*> ListaNavesKamikase;
	TArray<ANave_CMN*> ListaNavesCMN;
	TArray<ARobotFrancotirador*> ListaFrancotiradores;
	TArray<ABossEstatico*> ListaBossEstaticos;

	UPROPERTY()
		class UFacadeGeneradorNiveles* GerenteDeNiveles;

	int32 NivelAIniciar;
	FString DificultadActual;

private:
	void IniciarPartida();
	void AplicarDificultadEnemigo(AActor* Enemigo);
};