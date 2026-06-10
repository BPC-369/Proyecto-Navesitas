#pragma once

#include "CoreMinimal.h"
#include "GalagaModificadoMacProjectile.h"
#include "BombaRacimo.generated.h"

UCLASS()
class ABombaRacimo : public AGalagaModificadoMacProjectile
{
	GENERATED_BODY()

public:
	ABombaRacimo();

protected:
	virtual void BeginPlay() override;

	// Sobrescribe el OnHit del padre
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit) override;

private:
	void Explotar();
	FTimerHandle TimerExplosion;
};