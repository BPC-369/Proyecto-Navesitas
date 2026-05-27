#include "GalagaModificadoMacGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Asegúrate de tener los includes reales de tus clases
#include "NaveComando.h"
#include "Torreta.h"
#include "Robot_Lider.h"
#include "NaveLider.h"
#include "NaveKamikase.h"
#include "Nave_CMN.h"
#include "RobotFrancotirador.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	// Aquí modificas las cantidades cuando quieras (Sin tocar el Editor)
	CantNaveComando = 3;
	CantTorreta = 0;
	CantRobotLider = 0;
	CantNaveLider = 3;
	CantNaveKamikase = 3;
	CantNaveCMN = 3;
	CantFrancotirador = 0;
}

void AGalagaModificadoMacGameMode::BeginPlay()
{
	Super::BeginPlay();
	// Obtener el controlador del jugador
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		// Forzar modo "solo juego" (sin necesidad de clic)
		PC->SetInputMode(FInputModeGameOnly());
		// Ocultar el cursor del ratón
		PC->bShowMouseCursor = false;
		// Desactivar eventos de ratón sobrantes
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;
	}
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
	FVector PosicionAerea(0.0f, 20000.0f, 1500.0f);    // Las naves nacen en el aire
	FVector PosicionTerrestre(0.0f, 0.0f, 120.0f); // Los robots y torretas en el suelo

	float Separacion = 1500.0f; // Distancia entre cada enemigo

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

	// 7. Spawn Robot Francotirador (3)
	for (int32 i = 0; i < CantFrancotirador; i++)
	{
		// Los colocamos alejados en el terreno terrestre para que aprovechen su rango
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
		ARobotFrancotirador* NuevoFrancotirador = Mundo->SpawnActor<ARobotFrancotirador>(ARobotFrancotirador::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoFrancotirador) ListaFrancotiradores.Add(NuevoFrancotirador);
	}

	UE_LOG(LogTemp, Warning, TEXT("¡Ejército Desplegado Exitosamente!"));
}