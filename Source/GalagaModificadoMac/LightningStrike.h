#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightningStrike.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USphereComponent;
class UAudioComponent;

UCLASS()
class GALAGAMODIFICADOMAC_API ALightningStrike : public AActor
{
    GENERATED_BODY()

public:
    ALightningStrike();

protected:
    virtual void BeginPlay() override;

public:
    // Radio de daño al jugador (más grande para abarcar más área)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning")
        float DamageRadius = 800.0f;

    // Daño al jugador
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning")
        float DamageAmount = 100.0f;

    // Escala del efecto Niagara (ahora 30x para que sea colosal)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning")
        FVector EffectScale = FVector(100.0f, 100.0f, 500.0f);

private:
    UPROPERTY(VisibleAnywhere)
        USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere)
        UNiagaraComponent* NiagaraEffect;

    UPROPERTY(VisibleAnywhere)
        UAudioComponent* AudioComponent;

    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);

    // Efecto Niagara (cargado desde ruta)
    UPROPERTY(EditDefaultsOnly, Category = "Lightning")
        UNiagaraSystem* LightningEffect;

    // Sonido de impacto (cargado desde ruta)
    UPROPERTY(EditDefaultsOnly, Category = "Lightning")
        USoundBase* ImpactSound;
};