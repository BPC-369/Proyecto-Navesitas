#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraComponent; // <-- Añadimos la clase de Niagara

UCLASS()
class GALAGAMODIFICADOMAC_API ABossProjectile : public AActor
{
    GENERATED_BODY()
public:
    ABossProjectile();
    void ConfigurarProyectil(float NuevaEscala, float NuevoDano, float NuevaVelocidad);
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Componentes")
        USphereComponent* ColisionEsfera;

    // Reemplazamos la Malla por el componente Niagara
    UPROPERTY(VisibleAnywhere, Category = "Componentes")
        UNiagaraComponent* EfectoNiagara;

    UPROPERTY(VisibleAnywhere, Category = "Componentes")
        UProjectileMovementComponent* ComponenteMovimiento;

    float DanoProyectil;
};