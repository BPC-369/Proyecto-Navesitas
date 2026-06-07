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

    // BeginPlay se usará para cargar las explosiones
    virtual void BeginPlay() override;
    virtual void Destroyed() override;

protected:
    UStaticMeshComponent* MallaEnemiga;
    UComponenteCombate* ComponenteCombate;

    // Efecto de explosión (se carga en BeginPlay)
    UPROPERTY()
        UParticleSystem* ExplosionEffect;

    // Sonido opcional de explosión
    UPROPERTY()
        USoundBase* ExplosionSound;

    // Escala de la explosión
    float ExplosionScale = 1.0f;

    // Ruta del efecto de explosión (se define en cada constructor)
    FString RutaExplosion;

    // Ruta del sonido de explosión (opcional)
    FString RutaSonidoExplosion;
};