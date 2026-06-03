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

	// Le agregamos el parámetro "Tipo" con un valor por defecto para no romper la herencia
	virtual AObstaculoDestruido* CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion) override;

	// NUEVO MÉTODO ESPECIALIZADO: Para pedirle un objeto específico a la fábrica
	AObstaculoDestruido* CrearObstaculoEspecifico(UWorld* Mundo, FVector Posicion, FRotator Rotacion, FString Tipo);

private:
	TArray<UStaticMesh*> MallasEdificios;
	TArray<UStaticMesh*> MallasRocas;
	TArray<UStaticMesh*> MallasArboles;
};