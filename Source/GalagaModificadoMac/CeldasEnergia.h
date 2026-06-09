#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CeldasEnergia.generated.h"

class ACeldaEnergia;

class ICeldaObserver
{
public:
    virtual ~ICeldaObserver() = default;
    virtual void NotificarCeldaDestruida(ACeldaEnergia* CeldaQueMurio) = 0;
};

UCLASS()
class GALAGAMODIFICADOMAC_API ACeldaEnergia : public AActor
{
    GENERATED_BODY()

public:
    ACeldaEnergia();
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void AsignarObservador(ICeldaObserver* Observador);

    float GetVida() const { return Vida; }
    float GetVidaMaxima() const { return 400.0f; }

protected:
    virtual void BeginPlay() override;
    virtual void Destroyed() override;

private:
    UPROPERTY(VisibleAnywhere)
        class USceneComponent* RaizCelda;

    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* MallaCelda;

    float Vida;
    bool bDestruida;
    ICeldaObserver* MiObservador;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        class UParticleSystem* ExplosionEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        class USoundBase* ExplosionSound;

    float ExplosionScale = 15.0f;
};