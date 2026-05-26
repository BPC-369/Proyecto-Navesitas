// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoFactory.h"

AObstaculoFactory::AObstaculoFactory()
{
	PrimaryActorTick.bCanEverTick = false;
}

AObstaculoDestruido* AObstaculoFactory::CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion)
{
	return nullptr;
}
