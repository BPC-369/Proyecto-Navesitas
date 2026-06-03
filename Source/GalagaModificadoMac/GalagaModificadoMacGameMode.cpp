#include "GalagaModificadoMacGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Includes de los sistemas enemigos de tus compañeros
#include "NaveComando.h"
#include "Torreta.h"
#include "Robot_Lider.h"
#include "NaveLider.h"
#include "NaveKamikase.h"
#include "Nave_CMN.h"
#include "RobotFrancotirador.h"

// El include de la Fachada unificada
#include "FacadeGeneradorNiveles.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	NivelAIniciar = 0; // Cambia aquí de 0 a 14 para saltar de nivel

	// Inicialización de seguridad en 0 (La Fachada los sobreescribe en CargarNivelPorIndice)
	CantNaveComando = 0;
	CantTorreta = 0;
	CantRobotLider = 0;
	CantNaveLider = 0;
	CantNaveKamikase = 0;
	CantNaveCMN = 0;
	CantFrancotirador = 0;
	AmbienteActual = 1;
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

	// 1. Instanciamos la Fachada unificada
	GerenteDeNiveles = NewObject<UFacadeGeneradorNiveles>(this);

	if (GerenteDeNiveles)
	{
		GerenteDeNiveles->Inicializar(GetWorld()); // Corregido según tu método "Inicializar" de la fachada

		// 2. Cargamos el mapa e inyectamos los conteos de enemigos específicos del nivel
		GerenteDeNiveles->CargarNivelPorIndice(NivelAIniciar);
	}

	// 3. Desplegamos el escuadrón usando los datos inyectados por la fachada
	GenerarEjercito();
}

void AGalagaModificadoMacGameMode::GenerarEjercito()
{
	UWorld* Mundo = GetWorld();
	if (!Mundo) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Limpiamos listas anteriores por si acaso
	ListaNavesComando.Empty();
	ListaTorretas.Empty();
	ListaRobotsLider.Empty();
	ListaNavesLider.Empty();
	ListaNavesKamikase.Empty();
	ListaNavesCMN.Empty();
	ListaFrancotiradores.Empty();

	// Posiciones de la grilla de tu equipo
	FVector PosicionAerea(0.0f, 0.0f, 800.0f);
	FVector PosicionTerrestre(0.0f, 0.0f, 120.0f);
	float Separacion = 300.0f;

	// 1. Spawn Nave Comando
	for (int32 i = 0; i < CantNaveComando; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 0.0f, 0.0f);
		ANaveComando* NuevaNave = Mundo->SpawnActor<ANaveComando>(ANaveComando::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaNave) ListaNavesComando.Add(NuevaNave);
	}

	// 2. Spawn Torretas
	for (int32 i = 0; i < CantTorreta; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 500.0f, 0.0f);
		ATorreta* NuevaTorreta = Mundo->SpawnActor<ATorreta>(ATorreta::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaTorreta) ListaTorretas.Add(NuevaTorreta);
	}

	// 3. Spawn Robot Lider
	for (int32 i = 0; i < CantRobotLider; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1000.0f, 0.0f);
		ARobot_Lider* NuevoRobot = Mundo->SpawnActor<ARobot_Lider>(ARobot_Lider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoRobot) ListaRobotsLider.Add(NuevoRobot);
	}

	// 4. Spawn Nave Lider
	for (int32 i = 0; i < CantNaveLider; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 1500.0f, 0.0f);
		ANaveLider* NuevaNaveLider = Mundo->SpawnActor<ANaveLider>(ANaveLider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaNaveLider) ListaNavesLider.Add(NuevaNaveLider);
	}

	// 5. Spawn Nave Kamikase
	for (int32 i = 0; i < CantNaveKamikase; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -500.0f, 0.0f);
		ANaveKamikase* NuevoKamikase = Mundo->SpawnActor<ANaveKamikase>(ANaveKamikase::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoKamikase) ListaNavesKamikase.Add(NuevoKamikase);
	}

	// 6. Spawn Nave CMN
	for (int32 i = 0; i < CantNaveCMN; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -1000.0f, 0.0f);
		ANave_CMN* NuevaCMN = Mundo->SpawnActor<ANave_CMN>(ANave_CMN::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevaCMN) ListaNavesCMN.Add(NuevaCMN);
	}

	// 7. Spawn Robot Francotirador
	for (int32 i = 0; i < CantFrancotirador; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
		ARobotFrancotirador* NuevoFrancotirador = Mundo->SpawnActor<ARobotFrancotirador>(ARobotFrancotirador::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoFrancotirador) ListaFrancotiradores.Add(NuevoFrancotirador);
	}

	UE_LOG(LogTemp, Warning, TEXT("¡Ejército Desplegado Exitosamente por la Fachada!"));
}