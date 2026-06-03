// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioCiudad.h"
#include "ObstaculoCiudadDestruidaFactory.h" 
#include "Engine/World.h"

AEscenarioCiudad::AEscenarioCiudad()
{
	// Valores por defecto en caso de que no se use el Builder
	CantidadEdificios = 100;
	CantidadArboles = 50;
	CantidadRocas = 20;
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

	// Usamos la fábrica que el Builder ya inyectó previamente en el padre
	AObstaculoCiudadDestruidaFactory* MiFabrica = Cast<AObstaculoCiudadDestruidaFactory>(FabricaObstaculos);
	if (!MiFabrica) return;

	float MargenMuro = 300.0f;

	// --- BUCLE 1: EDIFICIOS ---
	for (int32 i = 0; i < CantidadEdificios; i++)
	{
		float RndX = FMath::FRandRange((-AnchoX / 2.0f) + MargenMuro, (AnchoX / 2.0f) - MargenMuro);
		float RndY = FMath::FRandRange((-LargoY / 2.0f) + MargenMuro, (LargoY / 2.0f) - MargenMuro);
		FVector PosicionSpawn(RndX, RndY, 0.0f);
		FRotator RotacionAleatoria(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);

		MiFabrica->CrearObstaculoEspecifico(Mundo, PosicionSpawn, RotacionAleatoria, TEXT("Edificio"));
	}

	// --- BUCLE 2: ÁRBOLES ---
	for (int32 i = 0; i < CantidadArboles; i++)
	{
		float RndX = FMath::FRandRange((-AnchoX / 2.0f) + MargenMuro, (AnchoX / 2.0f) - MargenMuro);
		float RndY = FMath::FRandRange((-LargoY / 2.0f) + MargenMuro, (LargoY / 2.0f) - MargenMuro);
		FVector PosicionSpawn(RndX, RndY, 0.0f);
		FRotator RotacionAleatoria(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);

		MiFabrica->CrearObstaculoEspecifico(Mundo, PosicionSpawn, RotacionAleatoria, TEXT("Arbol"));
	}

	// --- BUCLE 3: ROCAS ---
	for (int32 i = 0; i < CantidadRocas; i++)
	{
		float RndX = FMath::FRandRange((-AnchoX / 2.0f) + MargenMuro, (AnchoX / 2.0f) - MargenMuro);
		float RndY = FMath::FRandRange((-LargoY / 2.0f) + MargenMuro, (LargoY / 2.0f) - MargenMuro);
		FVector PosicionSpawn(RndX, RndY, 0.0f);
		FRotator RotacionAleatoria(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);

		MiFabrica->CrearObstaculoEspecifico(Mundo, PosicionSpawn, RotacionAleatoria, TEXT("Roca"));
	}

	// NOTA: Ya no hacemos MiFabrica->Destroy() aquí porque la fábrica ahora la maneja el Builder 
	// y debe vivir durante todo el nivel para que tus compañeros puedan usarla.
}