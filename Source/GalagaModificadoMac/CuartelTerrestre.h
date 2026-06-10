#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CuartelTerrestre.generated.h"

class UStaticMeshComponent;
class UComponenteCombate;

UCLASS()
class GALAGAMODIFICADOMAC_API ACuartelTerrestre : public AActor
{
    GENERATED_BODY()

public:
    ACuartelTerrestre();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cuartel")
        float VidaMaxima = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cuartel")
        float IntervaloAparicion = 12.0f;      // Más espaciado

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cuartel")
        int32 MaxUnidades = 12;               // Menos robots en total

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cuartel")
        int32 UnidadesPorOleada = 3;          // Oleadas más pequeñas

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cuartel")
        float RadioSpawn = 3000.0f;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
    virtual void Destroyed() override;

private:
    UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* MallaCuartel;

    UPROPERTY(VisibleAnywhere)
        UComponenteCombate* ComponenteCombate;

    FTimerHandle TimerHandle_Spawn;

    int32 UnidadesGeneradas = 0;

    void SpawnOleada();

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        class UParticleSystem* ExplosionEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Sonidos")
        class USoundBase* ExplosionSound;
};