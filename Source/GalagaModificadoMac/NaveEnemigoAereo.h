#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h" // 1. Ahora hereda nativamente de Pawn
#include "NaveEnemigoAereo.generated.h"

class UComponenteCombate;
class UStaticMeshComponent;

UCLASS(Abstract)
class GALAGAMODIFICADOMAC_API ANaveEnemigoAereo : public APawn
{
	GENERATED_BODY()

public:
	ANaveEnemigoAereo();
	float Velocidad;
	float DanioAtaque;
	float FrecuenciaAtaque;
	bool bTieneEscudo;

	virtual void Tick(float DeltaSeconds) override;

	// Función de vuelo tridimensional (Polimorfismo)
	virtual void Volar(float DeltaSeconds);

	// Función de ataque
	virtual void Atacar();

	// 2. El interceptor de daño para enviarlo al chip
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UStaticMeshComponent* MallaEnemiga;
	UComponenteCombate* ComponenteCombate;
};