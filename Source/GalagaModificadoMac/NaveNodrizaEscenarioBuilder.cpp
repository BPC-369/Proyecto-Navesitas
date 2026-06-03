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
	EscenarioEnConstruccion->AnchoX = 30000.0f;
	EscenarioEnConstruccion->LargoY = 30000.0f;
	EscenarioEnConstruccion->AltoZ = 4000.0f;

	// Inyectamos las cantidades base por defecto usando el Cast
	AEscenarioNaveNodriza* Nodriza = Cast<AEscenarioNaveNodriza>(EscenarioEnConstruccion);
	if (Nodriza)
	{
		Nodriza->CantidadPasillos = 60;
		Nodriza->CantidadTorretas = 15;
	}
}

void ANaveNodrizaEscenarioBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	// Como estamos DENTRO de una nave, no deberíamos ver estrellas ni nubes.
	// Podemos usar un material de rejilla metálica oscura o paneles para el techo.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialTechoAsset(TEXT("Material'/Game/Modelos/navenodriza/Mnavenodriza.Mnavenodriza'"));
	if (MaterialTechoAsset.Succeeded() && EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialTechoAsset.Object);
	}

	if (EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(1500.0f));
	}
}

void ANaveNodrizaEscenarioBuilder::ConstruirFisicasSuelo()
{
	if (!EscenarioEnConstruccion) return;

	// El suelo de la nave SÍ debe ser visible para que el jugador sienta el metal bajo la nave
	if (EscenarioEnConstruccion->Suelo)
	{
		EscenarioEnConstruccion->Suelo->SetVisibility(true);

		// Opcional: Si tienes un material de piso metálico, se lo puedes clavar aquí mismo
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialPisoAsset(TEXT("Material'/Game/Modelos/Nave/MPisoMetal.MPisoMetal'"));
		if (MaterialPisoAsset.Succeeded())
		{
			EscenarioEnConstruccion->Suelo->SetMaterial(0, MaterialPisoAsset.Object);
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
