#pragma once

#include "CoreMinimal.h"
#include "GalagaModificadoMacProjectile.h" // Importamos al padre
#include "BombaRacimo.generated.h"

UCLASS()
class ABombaRacimo : public AGalagaModificadoMacProjectile
{
	GENERATED_BODY()

public:
	ABombaRacimo();

protected:
	// Usamos BeginPlay para iniciar el temporizador de la explosión apenas nace
	virtual void BeginPlay() override;

public:
	// SOBRESCRIBIMOS la función del padre para que haga algo diferente al chocar
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
	void Explotar();
	FTimerHandle TimerExplosion;
};