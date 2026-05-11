#pragma once
#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "NaveComando.generated.h"

struct FDamageEvent;

UCLASS()
class GALAGAMODIFICADOMAC_API ANaveComando : public ANaveEnemigoAereo
{
	GENERATED_BODY()

public:
	ANaveComando();

protected:
	virtual void BeginPlay() override;

	// Firma exacta de la funci?n de da?o
	//virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual void Atacar() override;

private:
	void GestionarEscoltas();

	FTimerHandle TimerAtaque;
	FTimerHandle TimerSpawn;

	TArray<ANaveEnemigoAereo*> EscoltasActivas;

	const int32 MAX_ESCOLTAS = 21;
	const int32 NAVES_POR_SPAWN = 3;
};