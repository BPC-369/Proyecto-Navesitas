// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioNaveNodriza.h"
#include "ObstaculoNaveNodrizaFactory.h"
#include "Engine/World.h"

AEscenarioNaveNodriza::AEscenarioNaveNodriza()
{
	// Valores por defecto seguros
	CantidadPasillos = 40;
	CantidadTorretas = 10;
}

void AEscenarioNaveNodriza::BeginPlay()
{
	Super::BeginPlay();
}

void AEscenarioNaveNodriza::GenerarObstaculosProcedurales()
{
	UWorld* Mundo = GetWorld();
	if (!Mundo) return;

	AObstaculoNaveNodrizaFactory* MiFabrica = Cast<AObstaculoNaveNodrizaFactory>(FabricaObstaculos);
	if (!MiFabrica) return;

	float Margen = 400.0f;

	for (int32 i = 0; i < CantidadPasillos; i++)
	{
		float RndX = FMath::FRandRange((-AnchoX / 2.0f) + Margen, (AnchoX / 2.0f) - Margen);
		float RndY = FMath::FRandRange((-LargoY / 2.0f) + Margen, (LargoY / 2.0f) - Margen);

		FVector PosicionSpawn(RndX, RndY, 0.0f);
		FRotator Rotacion(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);

		MiFabrica->CrearObstaculoEspecifico(Mundo, PosicionSpawn, Rotacion, TEXT("Pasillo"));
	}

	for (int32 i = 0; i < CantidadTorretas; i++)
	{
		float RndX = FMath::FRandRange((-AnchoX / 2.0f) + Margen, (AnchoX / 2.0f) - Margen);
		float RndY = FMath::FRandRange((-LargoY / 2.0f) + Margen, (LargoY / 2.0f) - Margen);

		FVector PosicionSpawn(RndX, RndY, 50.0f);
		FRotator Rotacion(0.0f, 0.0f, 0.0f);

		MiFabrica->CrearObstaculoEspecifico(Mundo, PosicionSpawn, Rotacion, TEXT("Torreta"));
	}
}

