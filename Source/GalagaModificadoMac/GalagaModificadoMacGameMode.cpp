#include "GalagaModificadoMacGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GalagaModificadoMacPawn.h"
#include "UObject/ConstructorHelpers.h"

#include "NaveComando.h"
#include "Torreta.h"
#include "Robot_Lider.h"
#include "NaveLider.h"
#include "NaveKamikase.h"
#include "Nave_CMN.h"
#include "RobotFrancotirador.h"
#include "BossEstatico.h"

#include "FacadeGeneradorNiveles.h"
#include "ComponenteCombate.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	NivelAIniciar = 1;
	DificultadActual = TEXT("Normal");

	static ConstructorHelpers::FClassFinder<APawn> NaveBP(TEXT("/Game/Blueprints/BP_NaveJugador"));
	if (NaveBP.Succeeded())
	{
		DefaultPawnClass = NaveBP.Class;
	}
}

void AGalagaModificadoMacGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Solo guardamos los parámetros, no tocamos nada más
	FString LevelIndexStr = UGameplayStatics::ParseOption(Options, TEXT("LevelIndex"));
	if (!LevelIndexStr.IsEmpty())
	{
		NivelAIniciar = FCString::Atoi(*LevelIndexStr);
	}

	FString DifficultyStr = UGameplayStatics::ParseOption(Options, TEXT("Difficulty"));
	if (!DifficultyStr.IsEmpty())
	{
		DificultadActual = DifficultyStr;
	}
}

void AGalagaModificadoMacGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 1. Configurar el controlador (como siempre)
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
		PC->bEnableClickEvents = false;
		PC->bEnableMouseOverEvents = false;
	}

	// 2. Generar el nivel con la fachada y dificultad ya guardada
	GerenteDeNiveles = NewObject<UFacadeGeneradorNiveles>(this);
	if (GerenteDeNiveles)
	{
		GerenteDeNiveles->Inicializar(GetWorld());
		GerenteDeNiveles->CargarNivelPorIndice(NivelAIniciar);
		GerenteDeNiveles->AplicarDificultad(DificultadActual);

		FConfiguracionNivel DatosNivel = GerenteDeNiveles->GetConfiguracionActual();
		GenerarEjercito(DatosNivel.EnemigosPorGenerar);
	}
}

void AGalagaModificadoMacGameMode::AplicarDificultadEnemigo(AActor* Enemigo)
{
	if (!Enemigo || !GerenteDeNiveles) return;

	UComponenteCombate* Comp = Enemigo->FindComponentByClass<UComponenteCombate>();
	if (Comp)
	{
		Comp->VidaMaxima *= GerenteDeNiveles->MultiplicadorDificultad;
		Comp->VidaActual = Comp->VidaMaxima;
		Comp->MultiplicadorDano = GerenteDeNiveles->MultiplicadorDificultad;
	}
}

void AGalagaModificadoMacGameMode::GenerarEjercito(TMap<int32, int32> EnemigosDelNivel)
{
	UWorld* Mundo = GetWorld();
	if (!Mundo) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector PosicionAerea(100.0f, 9000.0f, 1500.0f);
	FVector PosicionTerrestre(0.0f, 0.0f, 120.0f);
	float Separacion = 3000.0f;

	int32 L_CantComando = EnemigosDelNivel.Contains(1) ? EnemigosDelNivel[1] : 0;
	int32 L_CantTorreta = EnemigosDelNivel.Contains(2) ? EnemigosDelNivel[2] : 0;
	int32 L_CantRobotLider = EnemigosDelNivel.Contains(3) ? EnemigosDelNivel[3] : 0;
	int32 L_CantNaveLider = EnemigosDelNivel.Contains(4) ? EnemigosDelNivel[4] : 0;
	int32 L_CantKamikase = EnemigosDelNivel.Contains(5) ? EnemigosDelNivel[5] : 0;
	int32 L_CantCMN = EnemigosDelNivel.Contains(6) ? EnemigosDelNivel[6] : 0;
	int32 L_CantFrancotirador = EnemigosDelNivel.Contains(7) ? EnemigosDelNivel[7] : 0;
	int32 L_CantBossEstatico = EnemigosDelNivel.Contains(8) ? EnemigosDelNivel[8] : 0;

	for (int32 i = 0; i < L_CantComando; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 0.0f, 0.0f);
		ANaveComando* N = Mundo->SpawnActor<ANaveComando>(ANaveComando::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (N) { ListaNavesComando.Add(N); AplicarDificultadEnemigo(N); }
	}
	for (int32 i = 0; i < L_CantTorreta; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 500.0f, 0.0f);
		ATorreta* T = Mundo->SpawnActor<ATorreta>(ATorreta::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (T) { ListaTorretas.Add(T); AplicarDificultadEnemigo(T); }
	}
	for (int32 i = 0; i < L_CantRobotLider; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1000.0f, 0.0f);
		ARobot_Lider* R = Mundo->SpawnActor<ARobot_Lider>(ARobot_Lider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (R) { ListaRobotsLider.Add(R); AplicarDificultadEnemigo(R); }
	}
	for (int32 i = 0; i < L_CantNaveLider; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, 1500.0f, 0.0f);
		ANaveLider* NL = Mundo->SpawnActor<ANaveLider>(ANaveLider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NL) { ListaNavesLider.Add(NL); AplicarDificultadEnemigo(NL); }
	}
	for (int32 i = 0; i < L_CantKamikase; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -500.0f, 0.0f);
		ANaveKamikase* K = Mundo->SpawnActor<ANaveKamikase>(ANaveKamikase::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (K) { ListaNavesKamikase.Add(K); AplicarDificultadEnemigo(K); }
	}
	for (int32 i = 0; i < L_CantCMN; i++)
	{
		FVector Pos = PosicionAerea + FVector(i * Separacion, -1000.0f, 0.0f);
		ANave_CMN* C = Mundo->SpawnActor<ANave_CMN>(ANave_CMN::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (C) { ListaNavesCMN.Add(C); AplicarDificultadEnemigo(C); }
	}
	for (int32 i = 0; i < L_CantFrancotirador; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
		ARobotFrancotirador* F = Mundo->SpawnActor<ARobotFrancotirador>(ARobotFrancotirador::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (F) { ListaFrancotiradores.Add(F); AplicarDificultadEnemigo(F); }
	}
	// 8. BossEstatico
	for (int32 i = 0; i < L_CantBossEstatico; i++)
	{
		FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
		ABossEstatico* NuevoBossEstatico = Mundo->SpawnActor<ABossEstatico>(ABossEstatico::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
		if (NuevoBossEstatico) ListaBossEstaticos.Add(NuevoBossEstatico);
	}

	UE_LOG(LogTemp, Warning, TEXT("¡Ejército desplegado con dificultad aplicada!"));
}