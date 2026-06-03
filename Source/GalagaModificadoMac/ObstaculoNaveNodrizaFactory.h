// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaculoFactory.h"
#include "ObstaculoNaveNodrizaFactory.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoNaveNodrizaFactory : public AObstaculoFactory
{
	GENERATED_BODY()

public:
	AObstaculoNaveNodrizaFactory();

	// Cumplimos con el contrato del padre
	virtual AObstaculoDestruido* CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion) override;

	// Nuestro método especializado para el Builder parametrizado
	AObstaculoDestruido* CrearObstaculoEspecifico(UWorld* Mundo, FVector Posicion, FRotator Rotacion, FString Tipo);

private:
	// Contenedores para tus assets tecnológicos
	UPROPERTY()
	TArray<UStaticMesh*> MallasPasillos;

	UPROPERTY()
	TArray<UStaticMesh*> MallasTorretas;

	UPROPERTY()
	TArray<UStaticMesh*> MallasCompuertas;
};