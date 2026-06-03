// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioEspacio.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaculoEspacioFactory.h" 
#include "Engine/World.h"

AEscenarioEspacio::AEscenarioEspacio()
{
	// ¡TOTALMENTE VACÍO! 
}

void AEscenarioEspacio::BeginPlay()
{
	Super::BeginPlay();

	// Arranca el gameplay y disparamos la siembra de meteoritos
	GenerarEntornoEspacial();
}

void AEscenarioEspacio::GenerarEntornoEspacial()
{
	// FabricaObstaculos ahora es rellenada por el Builder antes del BeginPlay
	if (!FabricaObstaculos || !GetWorld()) return;

	int32 CantidadMeteoritos = 1000;

	for (int32 i = 0; i < CantidadMeteoritos; i++)
	{
		// Tus matemáticas originales funcionan igual porque el Builder ya le dio valor a AnchoX, LargoY y AltoZ
		float RndX = FMath::FRandRange(-AnchoX / 2.0f, AnchoX / 2.0f);
		float RndY = FMath::FRandRange(-LargoY / 2.0f, LargoY / 2.0f);
		float RndZ = FMath::FRandRange(-AltoZ / 2.0f, AltoZ / 2.0f);

		FVector PosicionSpawn(RndX, RndY, RndZ);
		FRotator RotacionInicial(0.0f, 0.0f, 0.0f);

		// Llama a tu fábrica inyectada dinámicamente
		FabricaObstaculos->CrearObstaculo(GetWorld(), PosicionSpawn, RotacionInicial);
	}
}