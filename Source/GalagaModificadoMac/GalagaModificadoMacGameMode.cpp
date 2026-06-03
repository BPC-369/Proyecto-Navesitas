#include "GalagaModificadoMacGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Includes de los sistemas enemigos
#include "NaveComando.h"
#include "Torreta.h"
#include "Robot_Lider.h"
#include "NaveLider.h"
#include "NaveKamikase.h"
#include "Nave_CMN.h"
#include "RobotFrancotirador.h"

// Include del sistema de niveles unificado (Fachada)
#include "FacadeGeneradorNiveles.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
    // ----------------------------------------------------------------------
    // ¡CONTROLES DE LA CAMPAÑA DE 15 NIVELES!
    // ----------------------------------------------------------------------
    // Modifica este índice para testear cualquier nivel directamente (0 al 14).
    NivelAIniciar = 0;

    // Valores de oleada según tu configuración de la rama
    CantNaveComando = 3;
    CantTorreta = 0;
    CantRobotLider = 0;
    CantNaveLider = 6;
    CantNaveKamikase = 0;
    CantNaveCMN = 3;
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

        // 2. Carga el nivel correspondiente de la campaña usando la Fachada
        GerenteDeNiveles->CargarNivelPorIndice(NivelAIniciar);
    }

    // 3. Desplegamos tu escuadrón de naves con tus posiciones y separaciones
    GenerarEjercito();
}

void AGalagaModificadoMacGameMode::GenerarEjercito()
{
    UWorld* Mundo = GetWorld();
    if (!Mundo) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Posiciones y separación de tu rama (3000 de distancia, naves en el aire alejadas)
    FVector PosicionAerea(0.0f, 20000.0f, 1500.0f);
    FVector PosicionTerrestre(0.0f, 0.0f, 120.0f);
    float Separacion = 3000.0f;

    // 1. Nave Comando
    for (int32 i = 0; i < CantNaveComando; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, 0.0f, 0.0f);
        ANaveComando* NuevaNave = Mundo->SpawnActor<ANaveComando>(ANaveComando::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NuevaNave) ListaNavesComando.Add(NuevaNave);
    }

    // 2. Torretas
    for (int32 i = 0; i < CantTorreta; i++)
    {
        FVector Pos = PosicionTerrestre + FVector(i * Separacion, 500.0f, 0.0f);
        ATorreta* NuevaTorreta = Mundo->SpawnActor<ATorreta>(ATorreta::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NuevaTorreta) ListaTorretas.Add(NuevaTorreta);
    }

    // 3. Robot Lider
    for (int32 i = 0; i < CantRobotLider; i++)
    {
        FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1000.0f, 0.0f);
        ARobot_Lider* NuevoRobot = Mundo->SpawnActor<ARobot_Lider>(ARobot_Lider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NuevoRobot) ListaRobotsLider.Add(NuevoRobot);
    }

    // 4. Nave Lider
    for (int32 i = 0; i < CantNaveLider; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, 1500.0f, 0.0f);
        ANaveLider* NuevaNaveLider = Mundo->SpawnActor<ANaveLider>(ANaveLider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NuevaNaveLider) ListaNavesLider.Add(NuevaNaveLider);
    }

    // 5. Nave Kamikase
    for (int32 i = 0; i < CantNaveKamikase; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, -500.0f, 0.0f);
        ANaveKamikase* NuevoKamikase = Mundo->SpawnActor<ANaveKamikase>(ANaveKamikase::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NuevoKamikase) ListaNavesKamikase.Add(NuevoKamikase);
    }

    // 6. Nave CMN
    for (int32 i = 0; i < CantNaveCMN; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, -1000.0f, 0.0f);
        ANave_CMN* NuevaCMN = Mundo->SpawnActor<ANave_CMN>(ANave_CMN::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NuevaCMN) ListaNavesCMN.Add(NuevaCMN);
    }

    // 7. Robot Francotirador
    for (int32 i = 0; i < CantFrancotirador; i++)
    {
        FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
        ARobotFrancotirador* NuevoFrancotirador = Mundo->SpawnActor<ARobotFrancotirador>(ARobotFrancotirador::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NuevoFrancotirador) ListaFrancotiradores.Add(NuevoFrancotirador);
    }

    UE_LOG(LogTemp, Warning, TEXT("¡Ejército Desplegado Exitosamente!"));
}