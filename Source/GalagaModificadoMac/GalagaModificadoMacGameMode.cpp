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

// El include sagrado de tu Fachada unificada
#include "FacadeGeneradorNiveles.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	// ----------------------------------------------------------------------
	// ¡CONTROLES DE LA CAMPAÑA DE 15 NIVELES!
	// ----------------------------------------------------------------------
	// Modifica este índice para testear cualquier nivel directamente (0 al 14).
	// Ejemplo: 0 = Nivel 1 (Espacio), 3 = Nivel 4 (Ciudad), 9 = Nivel 10 (Nodriza).
	NivelAIniciar = 0;

	// Valores base por defecto (La Fachada se encargará del grueso en el futuro)
	CantNaveComando = 3;
	CantTorreta = 0;
	CantRobotLider = 0;
	CantNaveLider = 0;
	CantNaveKamikase = 0;
	CantNaveCMN = 0;
	CantFrancotirador = 0;
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

	// 1. Instanciamos al Gerente del Entorno (La Fachada unificada)
	GerenteDeNiveles = NewObject<UFacadeGeneradorNiveles>(this);

	if (GerenteDeNiveles)
	{
		GerenteDeNiveles->Inicializar(GetWorld());

		// 2. ¡ADIÓS AL SWITCH VIEJO! 
		// Ahora llamamos a tu método maestro pasándole la variable de índice de campaña.
		// La Fachada llamará al Director y al Builder correcto según el arreglo de 15 niveles.
		GerenteDeNiveles->CargarNivelPorIndice(NivelAIniciar);
	}

	// 3. Desplegamos el escuadrón de naves enemigas en el mapa cargado
	GenerarEjercito();
}

void AGalagaModificadoMacGameMode::GenerarEjercito()
{
	UWorld* Mundo = GetWorld();
	if (!Mundo) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Variables para separar a los enemigos en el mapa
	FVector PosicionAerea(0.0f, 20000.0f, 1500.0f);    // Las naves nacen en el aire
	FVector PosicionTerrestre(0.0f, 0.0f, 120.0f); // Los robots y torretas en el suelo

	float Separacion = 3000.0f; // Distancia entre cada enemigo

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

	UE_LOG(LogTemp, Warning, TEXT("¡Ejército Desplegado Exitosamente!"));
}