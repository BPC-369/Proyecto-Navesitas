#pragma once

#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "NaveKamikase.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ANaveKamikase : public ANaveEnemigoAereo
{
	GENERATED_BODY()
public:
	ANaveKamikase();
	virtual void Atacar() override;
	virtual void Explotar();

protected:
	// Evento nativo de Unreal que se dispara cuando este actor choca con algo que bloquea su camino
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	float RadioExplosion;
	float DanoExplosion;
};