#include "GalagaModificadoMacGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GalagaModificadoMacPawn.h"

// Includes de los sistemas enemigos de tus compañeros
#include "NaveComando.h"
#include "Torreta.h"
#include "Robot_Lider.h"
#include "NaveLider.h"
#include "NaveKamikase.h"
#include "Nave_CMN.h"
#include "RobotFrancotirador.h"

// El include sagrado de tu Fachada unificada
#include "FacadeGeneradorNiveles.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	// 1 = espacio, 2 = cudad, 3 = atmosfera, 4 = nave nodriza
	NivelAIniciar = 1;
	//DefaultPawnClass = AGalagaModificadoMacPawn::StaticClass();
}

void AGalagaModificadoMacGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Configuración del controlador del jugador
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;
	}

	GerenteDeNiveles = NewObject<UFacadeGeneradorNiveles>(this);

	if (GerenteDeNiveles)
	{
		GerenteDeNiveles->Inicializar(GetWorld());
		GerenteDeNiveles->CargarNivelPorIndice(NivelAIniciar);

		// AQUÍ SUCEDE LA MAGIA: Le pedimos a la Fachada la receta de enemigos de este nivel
		FConfiguracionNivel DatosNivel = GerenteDeNiveles->GetConfiguracionActual();

		// Se la pasamos al generador
		GenerarEjercito(DatosNivel.EnemigosPorGenerar);
	}
}

void AGalagaModificadoMacGameMode::GenerarEjercito(TMap<int32, int32> EnemigosDelNivel)
{
	UWorld* Mundo = GetWorld();
	if (!Mundo) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector PosicionAerea(0.0f, 20000.0f, 1500.0f);
	FVector PosicionTerrestre(0.0f, 0.0f, 120.0f);
	float Separacion = 3000.0f;

	// Leemos cuántos hay de cada uno de forma segura. Si el nivel no los pide, devuelve 0.
	int32 L_CantComando = EnemigosDelNivel.Contains(1) ? EnemigosDelNivel[1] : 0;
	int32 L_CantTorreta = EnemigosDelNivel.Contains(2) ? EnemigosDelNivel[2] : 0;
	int32 L_CantRobotLider = EnemigosDelNivel.Contains(3) ? EnemigosDelNivel[3] : 0;
	int32 L_CantNaveLider = EnemigosDelNivel.Contains(4) ? EnemigosDelNivel[4] : 0;
	int32 L_CantKamikase = EnemigosDelNivel.Contains(5) ? EnemigosDelNivel[5] : 0;
	int32 L_CantCMN = EnemigosDelNivel.Contains(6) ? EnemigosDelNivel[6] : 0;
	int32 L_CantFrancotirador = EnemigosDelNivel.Contains(7) ? EnemigosDelNivel[7] : 0;

	// 1. Spawn Nave Comando
	for (int32 i = 0; i < L_CantComando; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 0.0f, 0.0f);
		ANaveComando* NuevaNave = Mundo->SpawnActor<ANaveComando>(ANaveComando::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaNave) ListaNavesComando.Add(NuevaNave);
	}

	// 2. Spawn Torretas
	for (int32 i = 0; i < L_CantTorreta; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 500.0f, 0.0f);
		ATorreta* NuevaTorreta = Mundo->SpawnActor<ATorreta>(ATorreta::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaTorreta) ListaTorretas.Add(NuevaTorreta);
	}

	// 3. Spawn Robot Lider
	for (int32 i = 0; i < L_CantRobotLider; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1000.0f, 0.0f);
		ARobot_Lider* NuevoRobot = Mundo->SpawnActor<ARobot_Lider>(ARobot_Lider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoRobot) ListaRobotsLider.Add(NuevoRobot);
	}

	// 4. Spawn Nave Lider
	for (int32 i = 0; i < L_CantNaveLider; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 1500.0f, 0.0f);
		ANaveLider* NuevaNaveLider = Mundo->SpawnActor<ANaveLider>(ANaveLider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaNaveLider) ListaNavesLider.Add(NuevaNaveLider);
	}

	// 5. Spawn Nave Kamikase
	for (int32 i = 0; i < L_CantKamikase; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -500.0f, 0.0f);
		ANaveKamikase* NuevoKamikase = Mundo->SpawnActor<ANaveKamikase>(ANaveKamikase::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoKamikase) ListaNavesKamikase.Add(NuevoKamikase);
	}

	// 6. Spawn Nave CMN
	for (int32 i = 0; i < L_CantCMN; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -1000.0f, 0.0f);
		ANave_CMN* NuevaCMN = Mundo->SpawnActor<ANave_CMN>(ANave_CMN::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaCMN) ListaNavesCMN.Add(NuevaCMN);
	}

	// 7. Spawn Robot Francotirador
	for (int32 i = 0; i < L_CantFrancotirador; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
		ARobotFrancotirador* NuevoFrancotirador = Mundo->SpawnActor<ARobotFrancotirador>(ARobotFrancotirador::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoFrancotirador) ListaFrancotiradores.Add(NuevoFrancotirador);
	}

	UE_LOG(LogTemp, Warning, TEXT("¡Ejército Personalizado Desplegado Exitosamente!"));
}