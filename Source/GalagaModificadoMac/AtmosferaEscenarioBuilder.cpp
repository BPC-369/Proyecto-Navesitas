// Fill out your copyright notice in the Description page of Project Settings.


#include "AtmosferaEscenarioBuilder.h"
#include "ObstaculoAtmosferaFactory.h" 
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

void AAtmosferaEscenarioBuilder::ConstruirDimensiones()
{
	if (!EscenarioEnConstruccion) return;
	// Seteo externo de dimensiones migrado de tu constructor viejo
	EscenarioEnConstruccion->AnchoX = 100000.0f;
	EscenarioEnConstruccion->LargoY = 100000.0f;
	EscenarioEnConstruccion->AltoZ = 10000.0f;
}

void AAtmosferaEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	// Seteo del material del domo migrado de tu constructor viejo
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAtmosferaAsset(TEXT("Material'/Game/Modelos/Atmosfera/Mcieloapocaliptico.Mcieloapocaliptico'"));
	if (MaterialAtmosferaAsset.Succeeded() && EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialAtmosferaAsset.Object);
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
		EscenarioEnConstruccion->Suelo->SetVisibility(false);
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