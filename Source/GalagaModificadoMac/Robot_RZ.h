// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Robot_Razo.h"
#include "Robot_RZ.generated.h"

/**
 * 
 */
UCLASS()
class GALAGAMODIFICADOMAC_API ARobot_RZ : public ARobot_Razo
{
	GENERATED_BODY()
public:
	ARobot_RZ();

	virtual void Atacar() override;
protected:

	void EjecutarGolpeMelee();
	void ResetearAtaque();

	FTimerHandle TimerAtaque;
	FTimerHandle TimerCooldown;

	UPROPERTY(EditAnywhere, Category = "Animacion")
	UAnimMontage* AnimacionAtaqueMelee;
};
