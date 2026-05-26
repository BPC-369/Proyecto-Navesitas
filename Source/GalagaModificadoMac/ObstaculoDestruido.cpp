// Fill out your copyright notice in the Description page of Project Settings.

#include "ObstaculoDestruido.h"

AObstaculoDestruido::AObstaculoDestruido()
{
	PrimaryActorTick.bCanEverTick = false;
	MallaComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaComponent"));
	RootComponent = MallaComponent;
	MallaComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

