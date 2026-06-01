#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
// --- LA REGLA DE ORO DE UNREAL: Este debe ser el último include y coincidir con el nombre de tu archivo ---
#include "CeldasEnergia.generated.h"

class ACeldaEnergia; // Declaración adelantada

// --- INTERFAZ OBSERVER ACTUALIZADA ---
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
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void AsignarObservador(ICeldaObserver* Observador);

private:
    UPROPERTY(VisibleAnywhere)
        class UStaticMeshComponent* MallaCelda;

    float Vida;
    bool bDestruida;
    ICeldaObserver* MiObservador;
};