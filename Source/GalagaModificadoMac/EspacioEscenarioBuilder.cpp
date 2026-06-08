// Fill out your copyright notice in the Description page of Project Settings.


#include "EspacioEscenarioBuilder.h"
#include "EscenarioEspacio.h" 
#include "ObstaculoEspacioFactory.h" 
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"


AEspacioEscenarioBuilder::AEspacioEscenarioBuilder()
{
	MaterialEspacioGuardado = nullptr;
}


void AEspacioEscenarioBuilder::ConstruirDimensiones()
{
	if (!EscenarioEnConstruccion) return;

	EscenarioEnConstruccion->AnchoX = 100000.0f;
	EscenarioEnConstruccion->LargoY = 100000.0f;
	EscenarioEnConstruccion->AltoZ = 10000.0f;
}

void AEspacioEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	UMaterialInterface* MaterialEspacioAsset = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Modelos/MEspacioProfundo.MEspacioProfundo'"));

	if (EscenarioEnConstruccion->DomoCielo)
	{
		if (MaterialEspacioAsset)
		{
			EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialEspacioAsset);
		}

		EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(4000.0f));
		EscenarioEnConstruccion->DomoCielo->SetRelativeLocation(FVector(1000.0f));
	}
}

void AEspacioEscenarioBuilder::ConstruirFisicasSuelo()
{
	if (!EscenarioEnConstruccion) return;

	if (EscenarioEnConstruccion->Suelo)
	{
		EscenarioEnConstruccion->Suelo->SetVisibility(false);
	}
}

void AEspacioEscenarioBuilder::ConstruirFabricaDeObstaculos()
{
	if (!EscenarioEnConstruccion) return;

	UWorld* Mundo = GetWorld();
	if (Mundo)
	{
		FActorSpawnParameters SpawnParams;
		AObstaculoEspacioFactory* FabricaEspacio = Mundo->SpawnActor<AObstaculoEspacioFactory>(AObstaculoEspacioFactory::StaticClass(), SpawnParams);

		EscenarioEnConstruccion->FabricaObstaculos = FabricaEspacio;
	}
}

void AEspacioEscenarioBuilder::ConstruirFabricaDeEnemigos()
{
	if (!EscenarioEnConstruccion) return;

	//EscenarioEnConstruccion->FabricaEnemigos = nullptr;
}