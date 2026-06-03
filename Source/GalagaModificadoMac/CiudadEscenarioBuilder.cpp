// Fill out your copyright notice in the Description page of Project Settings.


#include "CiudadEscenarioBuilder.h"
#include "ObstaculoCiudadDestruidaFactory.h" // Tu fábrica de edificios
#include "EscenarioCiudad.h" // Tu clase de escenario urbano
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

void ACiudadEscenarioBuilder::ConstruirDimensiones()
{
	if (!EscenarioEnConstruccion) return;
	// Dimensiones de tu ciudad urbana
	EscenarioEnConstruccion->AnchoX = 20000.0f;
	EscenarioEnConstruccion->LargoY = 20000.0f;
	EscenarioEnConstruccion->AltoZ = 5000.0f;
	// ¡AQUÍ TIENES EL CONTROL TOTAL! Seteas las cantidades exactas desde el Builder
	AEscenarioCiudad* Ciudad = Cast<AEscenarioCiudad>(EscenarioEnConstruccion);
	if (Ciudad)
	{
		Ciudad->CantidadEdificios = 120; // Exactamente 120 edificios
		Ciudad->CantidadArboles = 65;    // Exactamente 65 árboles
		Ciudad->CantidadRocas = 15;      // Exactamente 15 rocas
	}
}

void ACiudadEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	// Tu material de fondo de ciudad destruida o cielo nublado urbano
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialCiudadAsset(TEXT("Material'/Game/Modelos/ciudaddestruida/Mciudad.Mciudad'"));
	if (MaterialCiudadAsset.Succeeded())
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialCiudadAsset.Object);
	}
	EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(1000.0f));
}

void ACiudadEscenarioBuilder::ConstruirFisicasSuelo()
{
	if (!EscenarioEnConstruccion) return;
	// En la ciudad el suelo SÍ se tiene que ver porque los edificios nacen de ahí
	if (EscenarioEnConstruccion->Suelo)
	{
		EscenarioEnConstruccion->Suelo->SetVisibility(true);
	}
}

void ACiudadEscenarioBuilder::ConstruirFabricaDeObstaculos()
{
	if (!EscenarioEnConstruccion) return;

	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		AObstaculoCiudadDestruidaFactory* FabricaCiudad = GetWorld()->SpawnActor<AObstaculoCiudadDestruidaFactory>(
			AObstaculoCiudadDestruidaFactory::StaticClass(), SpawnParams
		);

		EscenarioEnConstruccion->FabricaObstaculos = FabricaCiudad;
	}
}

void ACiudadEscenarioBuilder::ConstruirFabricaDeEnemigos()
{
	if (!EscenarioEnConstruccion) return;
	// Espacio en blanco para la fábrica de naves urbanas de tus compañeros
	//EscenarioEnConstruccion->FabricaEnemigos = nullptr;
}
