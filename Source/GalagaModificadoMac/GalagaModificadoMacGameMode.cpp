#include "GalagaModificadoMacGameMode.h"
#include "Engine/World.h"

// Asegúrate de tener los includes reales de tus clases
#include "NaveComando.h"
#include "Torreta.h"
#include "Robot_Lider.h"
#include "NaveLider.h"
#include "NaveKamikase.h"
#include "Nave_CMN.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	// Aquí modificas las cantidades cuando quieras (Sin tocar el Editor)
	CantNaveComando = 0;
	CantTorreta = 0;
	CantRobotLider = 0;
	CantNaveLider = 0;
	CantNaveKamikase = 0;
	CantNaveCMN = 0;
}

void AGalagaModificadoMacGameMode::BeginPlay()
{
	Super::BeginPlay();
	GenerarEjercito();
}

void AGalagaModificadoMacGameMode::GenerarEjercito()
{
	UWorld* Mundo = GetWorld();
	if (!Mundo) return;

	FActorSpawnParameters SpawnParams;
	// Obligamos a que nazcan incluso si se rozan un poco
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Variables para separar a los enemigos en el mapa
	FVector PosicionAerea(0.0f, 0.0f, 800.0f);    // Las naves nacen en el aire
	FVector PosicionTerrestre(0.0f, 0.0f, 120.0f); // Los robots y torretas en el suelo

	float Separacion = 300.0f; // Distancia entre cada enemigo

	// 1. Spawn Nave Comando (2)
	for (int32 i = 0; i < CantNaveComando; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 0.0f, 0.0f);
		ANaveComando* NuevaNave = Mundo->SpawnActor<ANaveComando>(ANaveComando::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaNave) ListaNavesComando.Add(NuevaNave);
	}

	// 2. Spawn Torretas (4)
	for (int32 i = 0; i < CantTorreta; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 500.0f, 0.0f);
		ATorreta* NuevaTorreta = Mundo->SpawnActor<ATorreta>(ATorreta::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaTorreta) ListaTorretas.Add(NuevaTorreta);
	}

	// 3. Spawn Robot Lider (1)
	for (int32 i = 0; i < CantRobotLider; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1000.0f, 0.0f);
		ARobot_Lider* NuevoRobot = Mundo->SpawnActor<ARobot_Lider>(ARobot_Lider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoRobot) ListaRobotsLider.Add(NuevoRobot);
	}

	// 4. Spawn Nave Lider (1)
	for (int32 i = 0; i < CantNaveLider; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 1500.0f, 0.0f);
		ANaveLider* NuevaNaveLider = Mundo->SpawnActor<ANaveLider>(ANaveLider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaNaveLider) ListaNavesLider.Add(NuevaNaveLider);
	}

	// 5. Spawn Nave Kamikase (3)
	for (int32 i = 0; i < CantNaveKamikase; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -500.0f, 0.0f);
		ANaveKamikase* NuevoKamikase = Mundo->SpawnActor<ANaveKamikase>(ANaveKamikase::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoKamikase) ListaNavesKamikase.Add(NuevoKamikase);
	}

	// 6. Spawn Nave CMN (5)
	for (int32 i = 0; i < CantNaveCMN; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -1000.0f, 0.0f);
		ANave_CMN* NuevaCMN = Mundo->SpawnActor<ANave_CMN>(ANave_CMN::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaCMN) ListaNavesCMN.Add(NuevaCMN);
	}

	UE_LOG(LogTemp, Warning, TEXT("¡Ejército Desplegado Exitosamente!"));
}