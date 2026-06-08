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

    TArray<ANaveComando*>      ListaNavesComando;
    TArray<ATorreta*>          ListaTorretas;
    TArray<ARobot_Lider*>      ListaRobotsLider;
    TArray<ANaveLider*>        ListaNavesLider;
    TArray<ANaveKamikase*>     ListaNavesKamikase;
    TArray<ANave_CMN*>         ListaNavesCMN;
    TArray<ARobotFrancotirador*> ListaFrancotiradores;
    TArray<ABossEstatico*>     ListaBossEstaticos;

    UPROPERTY()
        class UFacadeGeneradorNiveles* GerenteDeNiveles;

    int32 NivelAIniciar;
    FString DificultadActual;

    // Rotación del jefe (ajustable desde el editor o código)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jefe")
        FRotator RotacionJefe = FRotator(0.0f, 180.0f, 0.0f);

private:
    void AplicarDificultadEnemigo(AActor* Enemigo);

    UPROPERTY()
        UAudioComponent* MusicPlayer;

    UPROPERTY()
        TMap<int32, USoundBase*> MusicasAmbiente;

    UPROPERTY()
        USoundBase* MusicaJefeInvulnerable;

    UPROPERTY()
        USoundBase* MusicaJefeHeroica;

public:
    void IniciarMusica();
    void DetenerMusica();
    void CambiarMusicaJefe(int32 Fase);
};