// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaculoDestruido.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoDestruido : public AActor
{
	GENERATED_BODY()
public:
	AObstaculoDestruido();

	UPROPERTY(VisibleAnywhere, Category = "Obstaculo")
	UStaticMeshComponent* MallaComponent;
};
