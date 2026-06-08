#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NaveEnemigoAereo.generated.h"

class UComponenteCombate;
class UStaticMeshComponent;
class UParticleSystem;
class USoundBase;

UCLASS(Abstract)
class GALAGAMODIFICADOMAC_API ANaveEnemigoAereo : public APawn
{
    GENERATED_BODY()

public:
    ANaveEnemigoAereo();
    float Velocidad;
    float DanioAtaque;
    float FrecuenciaAtaque;
    bool  bTieneEscudo;

    virtual void Tick(float DeltaSeconds) override;
    virtual void Volar(float DeltaSeconds);
    virtual void Atacar();
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    virtual void BeginPlay() override;
    virtual void Destroyed() override;

    // Getter para el componente de combate (usado por el HUD)
    UComponenteCombate* GetComponenteCombate() const { return ComponenteCombate; }

protected:
    UStaticMeshComponent* MallaEnemiga;
    UComponenteCombate* ComponenteCombate;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        UParticleSystem* ExplosionEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        USoundBase* ExplosionSound;

    UPROPERTY(EditDefaultsOnly, Category = "Efectos")
        float ExplosionScale = 1.0f;

    FString RutaExplosion;
    FString RutaSonidoExplosion;
};