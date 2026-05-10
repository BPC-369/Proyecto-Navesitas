// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemigoTerrestre.h"
#include "Robot_Razo.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GALAGAMODIFICADOMAC_API ARobot_Razo : public AEnemigoTerrestre
{
	GENERATED_BODY()
public:
	ARobot_Razo();

	virtual void Tick(float DeltaTime) override;

protected:
	bool bProtocoloFuriaActivado; // Un seguro para que solo se cure una vez
};
