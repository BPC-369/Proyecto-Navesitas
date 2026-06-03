// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioDirector.h"

AEscenarioDirector::AEscenarioDirector()
{
	PrimaryActorTick.bCanEverTick = false;
}

AEscenarioBase* AEscenarioDirector::ConstruirEscenario(AEscenarioBuilder* BuilderElegido, TSubclassOf<AEscenarioBase> ClaseEscenario)
{
	if (!BuilderElegido || !ClaseEscenario) return nullptr;

	// Coreografía secuencial del patrón Builder
	BuilderElegido->InstanciarEscenario(ClaseEscenario);
	BuilderElegido->ConstruirDimensiones();
	BuilderElegido->ConstruirEsteticaCielo();
	BuilderElegido->ConstruirFisicasSuelo();
	BuilderElegido->ConstruirFabricaDeObstaculos();
	//BuilderElegido->ConstruirFabricaDeEnemigos(); // Espacio reservado para tus compañeros

	// Retornamos el nivel 100% equipado
	return BuilderElegido->GetEscenario();
}