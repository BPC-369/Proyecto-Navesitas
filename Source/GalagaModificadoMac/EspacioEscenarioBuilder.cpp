// Fill out your copyright notice in the Description page of Project Settings.


#include "EspacioEscenarioBuilder.h"
#include "ObstaculoEspacioFactory.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

void AEspacioEscenarioBuilder::ConstruirDimensiones()
{
	if (!EscenarioEnConstruccion) return;
	// Mudamos tus dimensiones originales
	EscenarioEnConstruccion->AnchoX = 100000.0f;
	EscenarioEnConstruccion->LargoY = 100000.0f;
	EscenarioEnConstruccion->AltoZ = 10000.0f;
}

void AEspacioEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	// Tu precarga del domo del espacio profundo se ejecuta aquí
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialEspacioAsset(TEXT("Material'/Game/Modelos/MEspacioProfundo.MEspacioProfundo'"));
	if (MaterialEspacioAsset.Succeeded())
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialEspacioAsset.Object);
	}

	EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(5000.0f));
	EscenarioEnConstruccion->DomoCielo->SetRelativeLocation(FVector(0.0f));
}

void AEspacioEscenarioBuilder::ConstruirFisicasSuelo()
{
	if (!EscenarioEnConstruccion) return;
	// Volvemos invisible el suelo como en tu código original
	if (EscenarioEnConstruccion->Suelo)
	{
		EscenarioEnConstruccion->Suelo->SetVisibility(false);
	}
}

void AEspacioEscenarioBuilder::ConstruirFabricaDeObstaculos()
{
	if (!EscenarioEnConstruccion) return;

	// Spawneamos tu fábrica del espacio y se la inyectamos al escenario base
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		AObstaculoEspacioFactory* FabricaEspacio = GetWorld()->SpawnActor<AObstaculoEspacioFactory>(
			AObstaculoEspacioFactory::StaticClass(), SpawnParams
		);

		EscenarioEnConstruccion->FabricaObstaculos = FabricaEspacio;
	}
}

void AEspacioEscenarioBuilder::ConstruirFabricaDeEnemigos()
{
	if (!EscenarioEnConstruccion) return;
	// Aquí es donde tus compas conectarán su fábrica de naves más adelante. 
	// De momento lo dejamos listo y protegido.
	//EscenarioEnConstruccion->FabricaEnemigos = nullptr;
}
