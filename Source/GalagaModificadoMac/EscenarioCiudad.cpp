// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioCiudad.h"
#include "ObstaculoCiudadDestruidaFactory.h" // Invocacion de la fabrica no eliminar)
#include "Engine/World.h"

AEscenarioCiudad::AEscenarioCiudad()
{
	//tamaño del escenario 
	AnchoX = 10000.0f;
	LargoY = 10000.0f;
	AltoZ = 1500.0f;
	GrosorPared = 50.0f;
	// cantidad de obstaculos a generar
	CantidadEdificios = 200; 
}

void AEscenarioCiudad::BeginPlay()
{
	Super::BeginPlay();
	GenerarObstaculosProcedurales();
}

void AEscenarioCiudad::GenerarObstaculosProcedurales()
{
	UWorld* Mundo = GetWorld();
	if (!Mundo) return;

	AObstaculoCiudadDestruidaFactory* MiFabrica = Mundo->SpawnActor<AObstaculoCiudadDestruidaFactory>(AObstaculoCiudadDestruidaFactory::StaticClass());
	if (!MiFabrica) return;

	float MargenMuro = 300.0f;

	for (int32 i = 0; i < CantidadEdificios; i++)
	{
		float RndX = FMath::FRandRange((-AnchoX / 2.0f) + MargenMuro, (AnchoX / 2.0f) - MargenMuro);
		float RndY = FMath::FRandRange((-LargoY / 2.0f) + MargenMuro, (LargoY / 2.0f) - MargenMuro);

		FVector PosicionSpawn(RndX, RndY, 0.0f);
		FRotator RotacionAleatoria(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);

		MiFabrica->CrearObstaculo(Mundo, PosicionSpawn, RotacionAleatoria);
	}

	MiFabrica->Destroy();
}