#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h" // 1. Ahora hereda de Character, no de EntidadCombate
#include "EnemigoTerrestre.generated.h"

// Declaración anticipada de tu chip
class UComponenteCombate;
class UStaticMeshComponent;

UCLASS(Abstract)
class GALAGAMODIFICADOMAC_API AEnemigoTerrestre : public ACharacter
{
	GENERATED_BODY()

protected:
	float VelocidadMovimiento;
	float RangoDeteccion;
	float DanioBase;
	float RangoAtaque;
	UStaticMeshComponent* MallaEnemiga;

	// 2. Declaramos tu Chip de Combate
	UComponenteCombate* ComponenteCombate;

public:
	AEnemigoTerrestre();

	virtual void Tick(float DeltaTime) override;
	virtual void Mover();
	virtual void ActualizarComportamiento();
	virtual bool DetectarObjetivo();
	virtual float CalcularDistanciaAlJugador();
	virtual void Atacar();

	// 3. Declaramos la intercepción de daño
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};