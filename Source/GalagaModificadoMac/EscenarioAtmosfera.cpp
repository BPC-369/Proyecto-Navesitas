// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioAtmosfera.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaculoAtmosferaFactory.h" 
#include "Engine/World.h"

AEscenarioAtmosfera::AEscenarioAtmosfera()
{
	CantidadNubesPiso = 500;
	CantidadMontanas = 25;

}

void AEscenarioAtmosfera::BeginPlay()
{
	Super::BeginPlay();

	GenerarPisoDeNubes();
	GenerarMontanas();
}

void AEscenarioAtmosfera::GenerarPisoDeNubes()
{
	AObstaculoAtmosferaFactory* FabricaAtmosfera = Cast<AObstaculoAtmosferaFactory>(FabricaObstaculos);
	if (!FabricaAtmosfera || !GetWorld()) return;

	for (int32 i = 0; i < CantidadNubesPiso; i++)
	{
		float RndX = FMath::FRandRange(-AnchoX / 2.0f, AnchoX / 2.0f);
		float RndY = FMath::FRandRange(-LargoY / 2.0f, LargoY / 2.0f);
		float Z_Piso = FMath::FRandRange(-4000.0f, -3500.0f);

		FVector PosicionSpawn(RndX, RndY, Z_Piso);

		float EscalaPlanaH = FMath::FRandRange(30.0f, 60.0f);
		float EscalaPlanaV = FMath::FRandRange(2.0f, 5.0f);

		// Ejecuta tu lógica original de spawn
		FabricaAtmosfera->CrearNubeEspecifica(GetWorld(), PosicionSpawn, EscalaPlanaH, EscalaPlanaH, EscalaPlanaV);
	}
}

void AEscenarioAtmosfera::GenerarMontanas()
{
	AObstaculoAtmosferaFactory* FabricaAtmosfera = Cast<AObstaculoAtmosferaFactory>(FabricaObstaculos);
	if (!FabricaAtmosfera || !GetWorld()) return;

	for (int32 i = 0; i < CantidadMontanas; i++)
	{
		float RndX = FMath::FRandRange(-AnchoX / 2.0f, AnchoX / 2.0f);
		float RndY = FMath::FRandRange(-LargoY / 2.0f, LargoY / 2.0f);
		float Z_Base = -4000.0f;

		FVector PosicionSpawn(RndX, RndY, Z_Base);

		FabricaAtmosfera->CrearMontanaEspecifica(GetWorld(), PosicionSpawn);
	}
}