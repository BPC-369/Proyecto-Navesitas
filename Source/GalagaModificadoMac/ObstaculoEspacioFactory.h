// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaculoFactory.h" 
#include "ObstaculoDestruido.h"
#include "ObstaculoEspacioFactory.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoEspacioFactory : public AObstaculoFactory
{
	GENERATED_BODY()

public:
	AObstaculoEspacioFactory();

	virtual AObstaculoDestruido* CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion) override;
};