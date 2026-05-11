// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GalagaModificadoMacProjectile.h" 
#include "BalaEspecial.generated.h" 
//hereda de la clase bala que viene por defento del unreal ya que ya lo tenemos trabajado para que haga danio
UCLASS()
class GALAGAMODIFICADOMAC_API ABalaEspecial : public AGalagaModificadoMacProjectile
{
	GENERATED_BODY()

public:
	ABalaEspecial();

protected:
	virtual void BeginPlay() override;

public:
	// Es vital tenerlo si quieres que la bala haga algo cada frame
	virtual void Tick(float DeltaTime) override;
};