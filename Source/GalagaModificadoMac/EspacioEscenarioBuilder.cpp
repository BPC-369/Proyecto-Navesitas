// Fill out your copyright notice in the Description page of Project Settings.


#include "EspacioEscenarioBuilder.h"
#include "EscenarioEspacio.h" 
#include "ObstaculoEspacioFactory.h" 
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"


AEspacioEscenarioBuilder::AEspacioEscenarioBuilder()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialEspacioAsset(TEXT("Material'/Game/Modelos/MEspacioProfundo.MEspacioProfundo'"));
	if (MaterialEspacioAsset.Succeeded())
	{
		MaterialEspacioGuardado = MaterialEspacioAsset.Object;
	}
	else
	{
		MaterialEspacioGuardado = nullptr;
	}
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

	if (EscenarioEnConstruccion->DomoCielo)
	{
		if (MaterialEspacioGuardado)
		{
			EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialEspacioGuardado);
		}

		EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(5000.0f));
		EscenarioEnConstruccion->DomoCielo->SetRelativeLocation(FVector(0.0f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DomoCielo es NULL en el EspacioEscenarioBuilder!"));
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
		// Instanciamos tu fábrica de meteoritos
		AObstaculoEspacioFactory* FabricaEspacio = Mundo->SpawnActor<AObstaculoEspacioFactory>(AObstaculoEspacioFactory::StaticClass(), SpawnParams);

		// Se la inyectamos al producto
		EscenarioEnConstruccion->FabricaObstaculos = FabricaEspacio;
	}
}

void AEspacioEscenarioBuilder::ConstruirFabricaDeEnemigos()
{
	if (!EscenarioEnConstruccion) return;

	// Espacio reservado para las naves de tus panas
	//EscenarioEnConstruccion->FabricaEnemigos = nullptr;
}