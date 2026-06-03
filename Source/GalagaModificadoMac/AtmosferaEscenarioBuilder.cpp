// Fill out your copyright notice in the Description page of Project Settings.


#include "AtmosferaEscenarioBuilder.h"
#include "ObstaculoAtmosferaFactory.h" 
#include "Components/StaticMeshComponent.h"
#include "EscenarioAtmosfera.h"
#include "UObject/ConstructorHelpers.h"


void AAtmosferaEscenarioBuilder::ConstruirDimensiones()
{
	if (!EscenarioEnConstruccion) return;
	EscenarioEnConstruccion->AnchoX = 100000.0f;
	EscenarioEnConstruccion->LargoY = 100000.0f;
	EscenarioEnConstruccion->AltoZ = 10000.0f;

	AEscenarioAtmosfera* Atmosfera = Cast<AEscenarioAtmosfera>(EscenarioEnConstruccion);
	if (Atmosfera)
	{
		Atmosfera->CantidadNubesPiso = CantidadA;
		Atmosfera->CantidadMontanas = CantidadB;
	}
}

void AAtmosferaEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	UMaterialInterface* MaterialAtmosferaAsset = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Modelos/Atmosfera/Mcieloapocaliptico.Mcieloapocaliptico'"));

	if (MaterialAtmosferaAsset && EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialAtmosferaAsset);
	}

	if (EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(10000.0f, 10000.0f, 10000.0f));
		EscenarioEnConstruccion->DomoCielo->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	}
}

void AAtmosferaEscenarioBuilder::ConstruirFisicasSuelo()
{
	if (!EscenarioEnConstruccion) return;
	// Desactivación del suelo migrado de tu constructor viejo
	if (EscenarioEnConstruccion->Suelo)
	{
		EscenarioEnConstruccion->Suelo->SetVisibility(true);
	}
}

void AAtmosferaEscenarioBuilder::ConstruirFabricaDeObstaculos()
{
	if (!EscenarioEnConstruccion) return;

	// Instanciamos dinámicamente tu fábrica específica de atmósfera en tiempo de ejecución
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		AObstaculoAtmosferaFactory* FabricaAtmosfera = GetWorld()->SpawnActor<AObstaculoAtmosferaFactory>(
			AObstaculoAtmosferaFactory::StaticClass(), SpawnParams
		);

		// Se la inyectamos al escenario base
		EscenarioEnConstruccion->FabricaObstaculos = FabricaAtmosfera;
	}
}

void AAtmosferaEscenarioBuilder::ConstruirFabricaDeEnemigos()
{
	if (!EscenarioEnConstruccion) return;
	// Reservado para tus compañeros
	//EscenarioEnConstruccion->FabricaEnemigos = nullptr;
}