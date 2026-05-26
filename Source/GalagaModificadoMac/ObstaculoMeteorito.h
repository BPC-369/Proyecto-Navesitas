// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaculoDestruido.h" // Cambiamos el include aquí
#include "ObstaculoMeteorito.generated.h"

UCLASS()

class GALAGAMODIFICADOMAC_API AObstaculoMeteorito : public AObstaculoDestruido
{
	GENERATED_BODY()

public:
	AObstaculoMeteorito();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	UStaticMeshComponent* MallaMeteorito;
};
