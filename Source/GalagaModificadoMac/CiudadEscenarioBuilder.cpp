// Fill out your copyright notice in the Description page of Project Settings.


#include "CiudadEscenarioBuilder.h"
#include "ObstaculoCiudadDestruidaFactory.h" 
#include "EscenarioCiudad.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

void ACiudadEscenarioBuilder::ConstruirDimensiones()
{
	if (!EscenarioEnConstruccion) return;
	EscenarioEnConstruccion->AnchoX = 100000.0f;
	EscenarioEnConstruccion->LargoY = 100000.0f;
	EscenarioEnConstruccion->AltoZ = 5000.0f;

	AEscenarioCiudad* Ciudad = Cast<AEscenarioCiudad>(EscenarioEnConstruccion);
	if (Ciudad)
	{
		Ciudad->CantidadEdificios = CantidadA; 
		Ciudad->CantidadArboles = CantidadB;   
		Ciudad->CantidadRocas = 15; 
	}
}

void ACiudadEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	UMaterialInterface* MaterialCiudadAsset = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Modelos/ciudaddestruida/Mciudad.Mciudad'"));

	if (MaterialCiudadAsset && EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialCiudadAsset);
	}

	if (EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(1000.0f));
	}
}

void ACiudadEscenarioBuilder::ConstruirFisicasSuelo()
{
	if (!EscenarioEnConstruccion) return;

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
	//EscenarioEnConstruccion->FabricaEnemigos = nullptr;
}
