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

    // Función inline para satisfacer al sistema de estados (evita LNK2019)
    void EjecutarAtaqueEstrategico(float DeltaTime = 0.0f) {}

    // Multiplicadores para los ataques (furia)
    float GetMultiplicadorDano() const { return MultiplicadorDano; }
    float GetMultiplicadorVelocidad() const { return MultiplicadorVelocidad; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(VisibleAnywhere)
        class UCapsuleComponent* CapsulaColision;

    UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* MallaJefe;

    IAttackStrategy* EstrategiaActual;

    float VidaMaxima;
    float VidaJefe;
    int32 CeldasActivas;
    bool bEscudoInmune;

    // Furia progresiva
    bool bFuriaCeldas;            // Activada al destruir todas las celdas
    bool bFuriaVida;              // Activada al bajar del 50% de vida
    float IntervaloCambio;        // Tiempo entre cambios de ataque
    float TiempoAcumuladoCambio;  // Contador para el cambio

    float MultiplicadorDano;
    float MultiplicadorVelocidad;

    void SeleccionarAtaqueAleatorioFuria();
};