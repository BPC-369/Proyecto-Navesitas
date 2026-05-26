// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoEdificio.h"

AObstaculoEdificio::AObstaculoEdificio()
{
	PrimaryActorTick.bCanEverTick = false;

	MallaEdificio = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEdificio"));
	RootComponent = MallaEdificio;

	// que tenga colisiones
	MallaEdificio->SetCollisionProfileName(TEXT("BlockAll"));
}

void AObstaculoEdificio::AsignarMalla(UStaticMeshComponent* MallaOrigen)
{
	if (MallaOrigen && MallaEdificio)
	{
		MallaEdificio->SetStaticMesh(MallaOrigen->GetStaticMesh());
		MallaEdificio->SetMaterial(0, MallaOrigen->GetMaterial(0));
	}
}

