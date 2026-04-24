#pragma once

#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "NaveKamikase.generated.h"

/**
 *
 */
UCLASS()
class GALAGAMODIFICADOMAC_API ANaveKamikase : public ANaveEnemigoAereo
{
	GENERATED_BODY()
public:
	ANaveKamikase();
	virtual void Volar(float DeltaSeconds) override;
	virtual void Atacar() override;
	virtual void Explotar();
};