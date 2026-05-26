// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaculoFactory.h"
#include "ObstaculoDestruido.h"
#include "ObstaculoAtmosferaFactory.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoAtmosferaFactory : public AObstaculoFactory
{
	GENERATED_BODY()

public:
	AObstaculoAtmosferaFactory();

	//heredado de ObstaculoFactory
	virtual AObstaculoDestruido* CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion) override;

	// para que se cree nubes y montanitas
	AObstaculoDestruido* CrearNubeEspecifica(UWorld* Mundo, FVector Posicion, float EscalaX, float EscalaY, float EscalaZ);
	AObstaculoDestruido* CrearMontanaEspecifica(UWorld* Mundo, FVector Posicion);
};