// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "ObstaculoFactory.h"
#include "ObstaculoCiudadDestruidaFactory.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoCiudadDestruidaFactory : public AObstaculoFactory
{
	GENERATED_BODY()
public:
	AObstaculoCiudadDestruidaFactory();

	virtual AObstaculoDestruido* CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion) override;

private:

	TArray<UStaticMesh*> MallasEdificios;
	TArray<UStaticMesh*> MallasRocas;
	TArray<UStaticMesh*> MallasArboles;
};