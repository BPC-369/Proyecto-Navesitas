// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoEdificio.h"

AObstaculoEdificio::AObstaculoEdificio()
{
	PrimaryActorTick.bCanEverTick = false;

	// que tenga colisiones
	MallaComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

void AObstaculoEdificio::AsignarMalla(UStaticMeshComponent* MallaOrigen)
{
	if (MallaOrigen && MallaComponent)
	{
		MallaComponent->SetStaticMesh(MallaOrigen->GetStaticMesh());
		MallaComponent->SetMaterial(0, MallaOrigen->GetMaterial(0));
	}
}

