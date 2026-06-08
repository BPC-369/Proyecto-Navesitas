// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Robot_Razo.h"
#include "Robot_RZ_D.generated.h"

/**
 * 
 */
UCLASS()
class GALAGAMODIFICADOMAC_API ARobot_RZ_D : public ARobot_Razo
{
	GENERATED_BODY()
public:
	ARobot_RZ_D();

	virtual void Atacar() override;
	virtual void Mover() override;
protected:
	FTimerHandle TimerDisparo;
	bool bArmaCargada;
	float DistanciaOptimaDeCombate;
	bool bPuedeDisparar;
	void ResetearDisparo();

	// Variable para poner tu animación de disparo desde el editor
	UPROPERTY(EditAnywhere, Category = "Animacion")
	UAnimMontage* AnimacionDisparo;
};
