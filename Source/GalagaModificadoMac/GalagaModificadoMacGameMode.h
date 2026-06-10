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
class ARobot_Medico;
class ARobot_RZ;
class ARobot_RZ_D;
class AFabricaInvulnerable;
class ACuartelTerrestre;

class UFacadeGeneradorNiveles;

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
    TArray<ARobot_Medico*> ListaMedicos;
    TArray<ARobot_RZ*> ListaRobotsRZ;
    TArray<ARobot_RZ_D*> ListaRobotsRZD;
    TArray<ACuartelTerrestre*> ListaCuarteles;

    UPROPERTY()
        class UFacadeGeneradorNiveles* GerenteDeNiveles;

    int32 NivelAIniciar;
    FString DificultadActual;
    void VerificarCondicionVictoria();
    int32 TiempoRestante;
    FTimerHandle TimerHandle_Reloj;

    void ActualizarContadorTiempo();
    void IniciarDerrotaPorTiempo();
    UFUNCTION()
        void OnEnemyDestroyed(AActor* DestroyedActor);
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
        void BossDerrotado();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jefe")
        FRotator RotacionJefe = FRotator(0.0f, 180.0f, 0.0f);

    UPROPERTY()
        TArray<AFabricaInvulnerable*> ListaFabricas;

    // ----- AJUSTES DE ALTURA PARA CUARTELES (modificables en editor) -----
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cuarteles")
        float AlturaBaseCuartel = 2500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cuarteles")
        float AjusteAlturaModeloCuartel = 500.0f;
    // ----------------------------------------------------------------------

    void SpawnCuarteles(int32 Cantidad);

    // ========== TORMENTA DE RELÁMPAGOS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tormenta")
        float IntervaloRelampagos = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tormenta")
        float RadioTormenta = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tormenta")
        float AlturaRelampagos = 2600.0f;

    FTimerHandle TimerHandle_Relampagos;

    void SpawnRelampago();
    void IniciarTormenta();
    void DetenerTormenta();
    // ============================================

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