// Fill out your copyright notice in the Description page of Project Settings.


#include "NaveNodrizaEscenarioBuilder.h"
#include "EscenarioNaveNodriza.h" // Cabecera del producto
#include "ObstaculoNaveNodrizaFactory.h" // Cabecera de su fábrica
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

void ANaveNodrizaEscenarioBuilder::ConstruirDimensiones()
{
	if (!EscenarioEnConstruccion) return;

	// Dimensiones cerradas y alargadas simulando el interior de una nave de asalto
	EscenarioEnConstruccion->AnchoX = 35000.0f;
	EscenarioEnConstruccion->LargoY = 35000.0f;
	EscenarioEnConstruccion->AltoZ = 45000.0f;

	AEscenarioNaveNodriza* Nodriza = Cast<AEscenarioNaveNodriza>(EscenarioEnConstruccion);
	if (Nodriza)
	{
		Nodriza->CantidadPasillos = CantidadA; // Ahora sí leerá tu '0' o lo que pongas en la Fachada
		Nodriza->CantidadTorretas = CantidadB; // Mapeado al obstáculo B
	}
}

void ANaveNodrizaEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	UMaterialInterface* MaterialTechoAsset = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Modelos/navenodriza/Mnavenodriza.Mnavenodriza'"));

	if (MaterialTechoAsset && EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialTechoAsset);
	}

	if (EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(2000.0f));
	}
}

void ANaveNodrizaEscenarioBuilder::ConstruirFisicasSuelo()
{
	if (!EscenarioEnConstruccion) return;

	if (EscenarioEnConstruccion->Suelo)
	{
		EscenarioEnConstruccion->Suelo->SetVisibility(true);

		UMaterialInterface* MaterialPisoAsset = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Modelos/Edificios/lambert98.lambert98'"));

		if (MaterialPisoAsset)
		{
			EscenarioEnConstruccion->Suelo->SetMaterial(0, MaterialPisoAsset);
		}
	}
}

void ANaveNodrizaEscenarioBuilder::ConstruirFabricaDeObstaculos()
{
	if (!EscenarioEnConstruccion) return;

	if (GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		// Instanciamos dinámicamente tu nueva fábrica de la Nave Nodriza
		AObstaculoNaveNodrizaFactory* FabricaNodriza = GetWorld()->SpawnActor<AObstaculoNaveNodrizaFactory>(
			AObstaculoNaveNodrizaFactory::StaticClass(), SpawnParams
		);

		// Se la inyectamos al escenario base
		EscenarioEnConstruccion->FabricaObstaculos = FabricaNodriza;
	}
}

void ANaveNodrizaEscenarioBuilder::ConstruirFabricaDeEnemigos()
{
	if (!EscenarioEnConstruccion) return;
	// Reservado para las naves de infiltración o torretas móviles de tus compañeros
	//EscenarioEnConstruccion->FabricaEnemigos = nullptr;
}
