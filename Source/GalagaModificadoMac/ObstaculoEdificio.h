// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaculoEdificio.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoEdificio : public AActor
{
	GENERATED_BODY()

public:
	AObstaculoEdificio();

	UPROPERTY(VisibleAnywhere, Category = "Obstaculo")
	UStaticMeshComponent* MallaEdificio;

	void AsignarMalla(UStaticMeshComponent* MallaOrigen);
};
