// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaculoDestruido.h"
#include "ObstaculoNube.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoNube : public AObstaculoDestruido
{
	GENERATED_BODY()

public:
	AObstaculoNube();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	UStaticMeshComponent* MallaNube;
};