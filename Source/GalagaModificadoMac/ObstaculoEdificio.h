// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstaculoDestruido.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaculoEdificio.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AObstaculoEdificio : public AObstaculoDestruido
{
	GENERATED_BODY()

public:
	AObstaculoEdificio();

	void AsignarMalla(UStaticMeshComponent* MallaOrigen);
};
