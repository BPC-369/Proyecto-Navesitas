// Fill out your copyright notice in the Description page of Project Settings.

#include "ObstaculoEspacioFactory.h"
#include "ObstaculoMeteorito.h"
#include "Engine/World.h"

AObstaculoEspacioFactory::AObstaculoEspacioFactory()
{
}

AObstaculoDestruido* AObstaculoEspacioFactory::CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion)
{
	// Si el mundo que nos pasan no existe, nos salimos
	if (!Mundo) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawneamos el meteorito usando el "Mundo" que viene por parámetro
	AObstaculoMeteorito* NuevoMeteorito = Mundo->SpawnActor<AObstaculoMeteorito>(
		AObstaculoMeteorito::StaticClass(),
		Posicion,
		Rotacion,
		SpawnParams
	);

	if (NuevoMeteorito)
	{
		// Escala de los metoritos 
		float EscalaAleatoria = FMath::FRandRange(0.4f, 3.5f);
		NuevoMeteorito->SetActorScale3D(FVector(EscalaAleatoria));

		FRotator RotacionEspacial(
			FMath::FRandRange(0.0f, 360.0f),
			FMath::FRandRange(0.0f, 360.0f),
			FMath::FRandRange(0.0f, 360.0f)
		);
		NuevoMeteorito->SetActorRotation(RotacionEspacial);

		return NuevoMeteorito;
	}

	return nullptr;
}