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
	// FabricaObstaculos ahora es rellenada por el Builder antes del BeginPlay
	if (!FabricaObstaculos || !GetWorld()) return;

	// Si la Fachada dice 100 creara 100
	int32 CantidadMeteoritos = CantidadObstaculosA;

	for (int32 i = 0; i < CantidadMeteoritos; i++)
	{
		// Tus matemáticas originales calculando el volumen del escenario
		float RndX = FMath::FRandRange(-AnchoX / 2.0f, AnchoX / 2.0f);
		float RndY = FMath::FRandRange(-LargoY / 2.0f, LargoY / 2.0f);
		float RndZ = FMath::FRandRange(-AltoZ / 2.0f, AltoZ / 2.0f);

		FVector PosicionSpawn(RndX, RndY, RndZ);

		// Opcional: Le metemos rotación aleatoria para que los meteoritos no salgan todos idénticos
		FRotator RotacionInicial(
			FMath::FRandRange(0.0f, 360.0f),
			FMath::FRandRange(0.0f, 360.0f),
			FMath::FRandRange(0.0f, 360.0f)
		);

		// Llama a tu fábrica inyectada dinámicamente
		FabricaObstaculos->CrearObstaculo(GetWorld(), PosicionSpawn, RotacionInicial);
	}
}