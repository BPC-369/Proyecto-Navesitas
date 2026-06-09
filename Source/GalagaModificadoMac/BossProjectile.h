#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "BossProjectile.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ABossProjectile : public AActor
{
    GENERATED_BODY()

public:
    ABossProjectile();

    virtual void Tick(float DeltaTime) override;

    void ConfigurarProyectil(float NuevaVelocidad, float NuevoDano, FVector Escala = FVector(1.0f));
    void SetDireccion(FVector Direccion);
    void SetRalentiza(bool bActivar) { bRalentiza = bActivar; }

    class USphereComponent* Colisionador;
    UProjectileMovementComponent* ComponenteMovimiento;
    float Dano;

    UFUNCTION()
        void AlEntrarEnColision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
        UNiagaraComponent* VFXNiagara;

    FVector SpawnLocation;
    bool bRalentiza = false;
    float MaxTravelDistance = 5000.0f;

    void AplicarRalentizacion(AActor* Victima);
};