// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioEspacio.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaculoEspacioFactory.h" 
#include "Engine/World.h"

AEscenarioEspacio::AEscenarioEspacio()
{

}

void AEscenarioEspacio::BeginPlay()
{
	Super::BeginPlay();

	if (Suelo)
	{
		Suelo->SetVisibility(false);
	}
}

void AEscenarioEspacio::GenerarEntornoEspacial()
{
	if (!FabricaObstaculos || !GetWorld()) return;

	int32 CantidadMeteoritos = 1000;

	for (int32 i = 0; i < CantidadMeteoritos; i++)
	{
		float RndX = FMath::FRandRange(-AnchoX / 2.0f, AnchoX / 2.0f);
		float RndY = FMath::FRandRange(-LargoY / 2.0f, LargoY / 2.0f);
		float RndZ = FMath::FRandRange(-AltoZ / 2.0f, AltoZ / 2.0f);

		FVector PosicionSpawn(RndX, RndY, RndZ);
		FRotator RotacionInicial(0.0f, 0.0f, 0.0f);

		FabricaObstaculos->CrearObstaculo(GetWorld(), PosicionSpawn, RotacionInicial);
	}
}