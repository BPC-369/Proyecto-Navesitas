#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "CeldasEnergia.h"
#include "AttackStrategies.h"
#include "BossEstatico.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ABossEstatico : public AActor, public ICeldaObserver
{
    GENERATED_BODY()

public:
    ABossEstatico();
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    virtual void NotificarCeldaDestruida(ACeldaEnergia* CeldaQueMurio) override;
    void CambiarEstrategia(IAttackStrategy* NuevaEstrategia);

    void EjecutarAtaqueEstrategico(float DeltaTime = 0.0f) {}

    float GetMultiplicadorDano() const { return MultiplicadorDano; }
    float GetMultiplicadorVelocidad() const { return MultiplicadorVelocidad; }

    // Vida del jefe (accesible desde el HUD)
    float VidaMaxima;
    float VidaJefe;

    // Número de celdas activas (público para las estrategias)
    int32 CeldasActivas;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Destroyed() override;

private:
    UPROPERTY(VisibleAnywhere)
        class UCapsuleComponent* CapsulaColision;

    UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* MallaJefe;

    IAttackStrategy* EstrategiaActual;

    bool bEscudoInmune;

    bool bFuriaCeldas;
    bool bFuriaVida;
    float IntervaloCambio;
    float TiempoAcumuladoCambio;

    float MultiplicadorDano;
    float MultiplicadorVelocidad;

    void SeleccionarAtaqueAleatorioFuria();

    // ========== EFECTOS VISUALES ==========
    UPROPERTY()
        class UParticleSystemComponent* EscudoPSC;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        class UParticleSystem* EscudoEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        class UParticleSystem* ExplosionEscudo;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        class UParticleSystem* ExplosionMuerte;

    float EscudoEffectScale = 40.0f;
    float ExplosionEscudoScale = 2.0f;
    float ExplosionMuerteScale = 30.0f;

    // ========== SONIDOS ==========
    UPROPERTY()
        class UAudioComponent* AudioComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Sonidos")
        class USoundBase* SoundRisa;

    UPROPERTY(EditDefaultsOnly, Category = "Sonidos")
        class USoundBase* SoundExplosionEscudo;

    UPROPERTY(EditDefaultsOnly, Category = "Sonidos")
        class USoundBase* SoundExplosionMuerte;

    FTimerHandle TimerHandle_Risa;
    float IntervaloRisa = 15.0f;

    void StartLaugh();
    void PlayLaugh();
    void StopLaugh();

    // ========== LLUVIA DE PROYECTILES ==========
    FTimerHandle TimerHandle_Lluvia;
    float IntervaloLluvia = 8.0f;

    void IniciarLluvia();
    void EjecutarLluvia();
};