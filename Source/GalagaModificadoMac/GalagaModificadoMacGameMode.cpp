// Copyright Epic Games, Inc. All Rights Reserved.

#include "GalagaModificadoMacGameMode.h"
#include "NaveLider.h"
#include "Torreta.h"
#include "GalagaModificadoMacPawn.h"
#include "Engine/World.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	DefaultPawnClass = AGalagaModificadoMacPawn::StaticClass();
}

void AGalagaModificadoMacGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		// --- SPAWN DE LA NAVE LÍDER ---
		FVector UbicacionNave(1500.0f, 0.0f, 600.0f);
		FRotator RotacionNave(0.0f, 180.0f, 0.0f);
		GetWorld()->SpawnActor<ANaveLider>(ANaveLider::StaticClass(), UbicacionNave, RotacionNave);

		// --- SPAWN DE LA TORRETA ESTRUCTURAL ---
		// X: Un poco más cerca, Y: Desplazada a la derecha, Z: Pegada al "suelo"
		FVector UbicacionTorreta(800.0f, 400.0f, 300.0f);
		FRotator RotacionTorreta(0.0f, 0.0f, 0.0f);

		ATorreta* NuevaTorreta = GetWorld()->SpawnActor<ATorreta>(ATorreta::StaticClass(), UbicacionTorreta, RotacionTorreta);

		if (NuevaTorreta)
		{
			UE_LOG(LogTemp, Warning, TEXT("Torreta Estructural desplegada en el campo de batalla."));
		}
	}
}
