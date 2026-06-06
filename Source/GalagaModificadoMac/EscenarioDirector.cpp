// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioDirector.h"

AEscenarioDirector::AEscenarioDirector()
{
	PrimaryActorTick.bCanEverTick = false;
}

AEscenarioBase* AEscenarioDirector::ConstruirEscenario(AEscenarioBuilder* BuilderElegido, TSubclassOf<AEscenarioBase> ClaseEscenario)
{
	if (!BuilderElegido || !ClaseEscenario) return nullptr;

	BuilderElegido->InstanciarEscenario(ClaseEscenario);
	BuilderElegido->ConstruirDimensiones();

	// ¡NUEVO! Expande los muros físicos a la escala real del Builder (Ej: 100000)
	if (BuilderElegido->GetEscenario())
	{
		BuilderElegido->GetEscenario()->AplicarEscalaFisica();
	}

	BuilderElegido->ConstruirEsteticaCielo();
	BuilderElegido->ConstruirFisicasSuelo();
	BuilderElegido->ConstruirFabricaDeObstaculos();

	return BuilderElegido->GetEscenario();
}