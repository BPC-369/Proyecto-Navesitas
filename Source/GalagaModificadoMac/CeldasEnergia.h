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

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
        class USceneComponent* RaizCelda;

    UPROPERTY()
        class USceneComponent* PivoteCentrado;   // Pivote que situaremos en el centro exacto

    UPROPERTY()
        class UStaticMeshComponent* MallaCelda;

    float Vida;
    bool bDestruida;
    ICeldaObserver* MiObservador;
};