#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FabricaInvulnerable.generated.h"

class UStaticMeshComponent;
class UParticleSystemComponent;

UCLASS()
class GALAGAMODIFICADOMAC_API AFabricaInvulnerable : public AActor
{
    GENERATED_BODY()

public:
    AFabricaInvulnerable();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
        UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
        UParticleSystemComponent* ParticleComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
        float IntervaloSpawn = 3.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
        int32 NavesPorSpawn = 2;

    // Altura del cuartel respecto al suelo (0 = a ras de suelo, >0 más alto, <0 más bajo)
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
        float AlturaZ = 0.0f;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
    FTimerHandle TimerHandle_Spawn;
    void SpawnNaves();
};