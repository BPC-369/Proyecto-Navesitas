#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CeldasEnergia.h" // AHORA INCLUYE ESTE
#include "BossEstatico.generated.h"

class UStaticMeshComponent;
class IBossState;
class IAttackStrategy;

UCLASS()
class GALAGAMODIFICADOMAC_API ABossEstatico : public AActor, public ICeldaObserver
{
    GENERATED_BODY()
public:
    ABossEstatico();
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // Actualizamos el parámetro
    virtual void NotificarCeldaDestruida(ACeldaEnergia* CeldaQueMurio) override;

    void CambiarEstado(IBossState* NuevoEstado);
    void CambiarEstrategia(IAttackStrategy* NuevaEstrategia);
    void EjecutarAtaqueEstrategico(float DeltaTime);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UStaticMeshComponent* MallaJefe;
    UStaticMeshComponent* EscudoMesh;

    IBossState* EstadoActual;
    IAttackStrategy* EstrategiaActual;

    float VidaCore;
    int CeldasActivas;
    bool bEscudoActivo;
};