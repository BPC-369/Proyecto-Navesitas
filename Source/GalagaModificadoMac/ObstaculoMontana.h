// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaculoDestruido.h"
#include "ObstaculoMontana.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoMontana : public AObstaculoDestruido
{
	GENERATED_BODY()

public:
	AObstaculoMontana();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	UStaticMeshComponent* MallaMontana;
};
