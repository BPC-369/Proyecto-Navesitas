#include "GalagaModificadoMacGameMode.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "GalagaModificadoMacPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

#include "NaveComando.h"
#include "Torreta.h"
#include "Robot_Lider.h"
#include "NaveLider.h"
#include "NaveKamikase.h"
#include "Nave_CMN.h"
#include "RobotFrancotirador.h"
#include "BossEstatico.h"
#include "Robot_Medico.h"
#include "Robot_RZ.h"
#include "Robot_RZ_D.h"
#include "CeldasEnergia.h"
#include "FabricaInvulnerable.h"

#include "FacadeGeneradorNiveles.h"
#include "GalagaGameInstance.h"
#include "ComponenteCombate.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"


AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
    NivelAIniciar = -1;
    DificultadActual = TEXT("Normal");

    static ConstructorHelpers::FClassFinder<APawn> NaveBP(TEXT("/Game/Blueprints/BP_NaveJugador"));
    if (NaveBP.Succeeded())
    {
        DefaultPawnClass = NaveBP.Class;
    }

    MusicPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicPlayer"));
    MusicPlayer->bAutoActivate = false;

    MusicasAmbiente.Add(1, LoadObject<USoundBase>(nullptr, TEXT("/Game/music/espacio_1")));
    MusicasAmbiente.Add(2, LoadObject<USoundBase>(nullptr, TEXT("/Game/music/jefe_2")));
    MusicasAmbiente.Add(3, LoadObject<USoundBase>(nullptr, TEXT("/Game/music/ciudad_1")));
    MusicasAmbiente.Add(4, LoadObject<USoundBase>(nullptr, TEXT("/Game/music/nave_1")));

    MusicaJefeInvulnerable = LoadObject<USoundBase>(nullptr, TEXT("/Game/music/atmosfera_1"));
    MusicaJefeHeroica = LoadObject<USoundBase>(nullptr, TEXT("/Game/music/jefe_2"));
}

void AGalagaModificadoMacGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

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
        GerenteDeNiveles->AplicarDificultad(DificultadActual);

        FConfiguracionNivel DatosNivel = GerenteDeNiveles->GetConfiguracionActual();

        TiempoRestante = DatosNivel.TiempoLimite;
        if ((NivelAIniciar == 3 || NivelAIniciar == 4 || NivelAIniciar == 5 || NivelAIniciar == 12) && TiempoRestante > 0)
        {
            GetWorldTimerManager().SetTimer(TimerHandle_Reloj, this, &AGalagaModificadoMacGameMode::ActualizarContadorTiempo, 1.0f, true);
        }

        GenerarEjercito(DatosNivel.EnemigosPorGenerar);
    }

    IniciarMusica();
}

void AGalagaModificadoMacGameMode::ActualizarContadorTiempo()
{
    if (TiempoRestante > 0)
    {
        TiempoRestante--;
        FString MsgTiempo = FString::Printf(TEXT("Tiempo Restante: %d s"), TiempoRestante);
        GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, MsgTiempo);
    }
    else
    {
        GetWorldTimerManager().ClearTimer(TimerHandle_Reloj);
        IniciarDerrotaPorTiempo();
    }
}

void AGalagaModificadoMacGameMode::IniciarDerrotaPorTiempo()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("¡TIEMPO AGOTADO! MISIÓN FALLIDA"));

    if (GetGameInstance())
    {
        UGalagaGameInstance* GI = Cast<UGalagaGameInstance>(GetGameInstance());
        if (GI)
        {
            GI->ReturnToMainMenu();
        }
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

    FVector PosicionAerea(100.0f, 11000.0f, 1500.0f);
    FVector PosicionTerrestre(0.0f, 0.0f, 2500.0f);
    float Separacion = 7000.0f;

    int32 L_CantComando = EnemigosDelNivel.Contains(1) ? EnemigosDelNivel[1] : 0;
    int32 L_CantTorreta = EnemigosDelNivel.Contains(2) ? EnemigosDelNivel[2] : 0;
    int32 L_CantRobotLider = EnemigosDelNivel.Contains(3) ? EnemigosDelNivel[3] : 0;
    int32 L_CantNaveLider = EnemigosDelNivel.Contains(4) ? EnemigosDelNivel[4] : 0;
    int32 L_CantKamikase = EnemigosDelNivel.Contains(5) ? EnemigosDelNivel[5] : 0;
    int32 L_CantCMN = EnemigosDelNivel.Contains(6) ? EnemigosDelNivel[6] : 0;
    int32 L_CantFrancotirador = EnemigosDelNivel.Contains(7) ? EnemigosDelNivel[7] : 0;
    int32 L_CantBossEstatico = EnemigosDelNivel.Contains(8) ? EnemigosDelNivel[8] : 0;
    int32 L_CantMedico = EnemigosDelNivel.Contains(9) ? EnemigosDelNivel[9] : 0;
    int32 L_CantRobotRZ = EnemigosDelNivel.Contains(10) ? EnemigosDelNivel[10] : 0;
    int32 L_CantRobotRZD = EnemigosDelNivel.Contains(11) ? EnemigosDelNivel[11] : 0;

    // --- Macro para enlazar el delegado de muerte a la función UFUNCTION ---
#define BIND_DESTROY(EnemyPtr) \
        { \
            FScriptDelegate Delegate; \
            Delegate.BindUFunction(this, FName("OnEnemyDestroyed")); \
            EnemyPtr->OnDestroyed.Add(Delegate); \
        }

    // Enemigos aéreos
    for (int32 i = 0; i < L_CantComando; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, 0.0f, 0.0f);
        ANaveComando* N = Mundo->SpawnActor<ANaveComando>(ANaveComando::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (N)
        {
            ListaNavesComando.Add(N);
            AplicarDificultadEnemigo(N);
            BIND_DESTROY(N);
        }
    }
    for (int32 i = 0; i < L_CantNaveLider; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, 1500.0f, 0.0f);
        ANaveLider* NL = Mundo->SpawnActor<ANaveLider>(ANaveLider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (NL)
        {
            ListaNavesLider.Add(NL);
            AplicarDificultadEnemigo(NL);
            BIND_DESTROY(NL);
        }
    }
    for (int32 i = 0; i < L_CantKamikase; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, -500.0f, 0.0f);
        ANaveKamikase* K = Mundo->SpawnActor<ANaveKamikase>(ANaveKamikase::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (K)
        {
            ListaNavesKamikase.Add(K);
            AplicarDificultadEnemigo(K);
            BIND_DESTROY(K);
        }
    }
    for (int32 i = 0; i < L_CantCMN; i++)
    {
        FVector Pos = PosicionAerea + FVector(i * Separacion, -1000.0f, 0.0f);
        ANave_CMN* C = Mundo->SpawnActor<ANave_CMN>(ANave_CMN::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (C)
        {
            ListaNavesCMN.Add(C);
            AplicarDificultadEnemigo(C);
            BIND_DESTROY(C);
        }
    }

    // Terrestres
    for (int32 i = 0; i < L_CantTorreta; i++)
    {
        FVector Pos = PosicionTerrestre + FVector(i * Separacion, 500.0f, 0.0f);
        ATorreta* T = Mundo->SpawnActor<ATorreta>(ATorreta::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (T)
        {
            ListaTorretas.Add(T);
            AplicarDificultadEnemigo(T);
            BIND_DESTROY(T);
        }
    }
    for (int32 i = 0; i < L_CantRobotLider; i++)
    {
        FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1000.0f, 0.0f);
        ARobot_Lider* R = Mundo->SpawnActor<ARobot_Lider>(ARobot_Lider::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (R)
        {
            ListaRobotsLider.Add(R);
            AplicarDificultadEnemigo(R);
            BIND_DESTROY(R);
        }
    }
    for (int32 i = 0; i < L_CantFrancotirador; i++)
    {
        FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
        ARobotFrancotirador* F = Mundo->SpawnActor<ARobotFrancotirador>(ARobotFrancotirador::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (F)
        {
            ListaFrancotiradores.Add(F);
            AplicarDificultadEnemigo(F);
            BIND_DESTROY(F);
        }
    }
    for (int32 i = 0; i < L_CantMedico; i++)
    {
        FVector Pos = FVector(PosicionAerea.X + (i * Separacion), PosicionAerea.Y, 100.0f);
        ARobot_Medico* M = Mundo->SpawnActor<ARobot_Medico>(ARobot_Medico::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (M)
        {
            ListaMedicos.Add(M);
            AplicarDificultadEnemigo(M);
            BIND_DESTROY(M);
        }
    }
    for (int32 i = 0; i < L_CantRobotRZ; i++)
    {
        FVector Pos = FVector(PosicionAerea.X + (i * Separacion), PosicionAerea.Y, 100.0f);
        ARobot_RZ* RZ = Mundo->SpawnActor<ARobot_RZ>(ARobot_RZ::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (RZ)
        {
            ListaRobotsRZ.Add(RZ);
            AplicarDificultadEnemigo(RZ);
            BIND_DESTROY(RZ);
        }
    }
    for (int32 i = 0; i < L_CantRobotRZD; i++)
    {
        FVector Pos = FVector(PosicionAerea.X + (i * Separacion), PosicionAerea.Y, 100.0f);
        ARobot_RZ_D* RZD = Mundo->SpawnActor<ARobot_RZ_D>(ARobot_RZ_D::StaticClass(), Pos, FRotator::ZeroRotator, SpawnParams);
        if (RZD)
        {
            ListaRobotsRZD.Add(RZD);
            AplicarDificultadEnemigo(RZD);
            BIND_DESTROY(RZD);
        }
    }

    // Celdas (solo jefe)
    if (NivelAIniciar == 12)
    {
        // Distancia mínima y máxima desde el centro (el jefe)
        float DistanciaMin = 5000.0f;
        float DistanciaMax = 10000.0f;
        float AlturaSuelo = 1290.0f;   // Mantén la altura de tu escenario

        // Tres ángulos base para separar las celdas (0°, 120°, 240°)
        TArray<float> AngulosBase = { 0.0f, 120.0f, 240.0f };

        for (int32 i = 0; i < 3; i++)
        {
            // Ángulo con una pequeña variación aleatoria para que no queden perfectamente alineadas
            float Angulo = AngulosBase[i] + FMath::RandRange(-20.0f, 20.0f);
            float Distancia = FMath::RandRange(DistanciaMin, DistanciaMax);

            float Rad = FMath::DegreesToRadians(Angulo);
            FVector PosCelda = FVector(
                Distancia * FMath::Cos(Rad),
                Distancia * FMath::Sin(Rad),
                AlturaSuelo
            );

            Mundo->SpawnActor<ACeldaEnergia>(ACeldaEnergia::StaticClass(), PosCelda, FRotator::ZeroRotator, SpawnParams);
        }
    }

    // Boss
    for (int32 i = 0; i < L_CantBossEstatico; i++)
    {
        FVector Pos = PosicionTerrestre + FVector(i * Separacion, 1500.0f, 0.0f);
        ABossEstatico* Boss = Mundo->SpawnActor<ABossEstatico>(ABossEstatico::StaticClass(), Pos, RotacionJefe, SpawnParams);
        if (Boss)
        {
            ListaBossEstaticos.Add(Boss);
            BIND_DESTROY(Boss);
        }
    }

#undef BIND_DESTROY

    UE_LOG(LogTemp, Warning, TEXT("¡Ejército desplegado con dificultad aplicada!"));
}

void AGalagaModificadoMacGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
    // Intentar eliminar el actor de la lista correspondiente usando un Cast seguro
    if (ANaveComando* N = Cast<ANaveComando>(DestroyedActor))
        ListaNavesComando.Remove(N);
    else if (ATorreta* T = Cast<ATorreta>(DestroyedActor))
        ListaTorretas.Remove(T);
    else if (ARobot_Lider* R = Cast<ARobot_Lider>(DestroyedActor))
        ListaRobotsLider.Remove(R);
    else if (ANaveLider* NL = Cast<ANaveLider>(DestroyedActor))
        ListaNavesLider.Remove(NL);
    else if (ANaveKamikase* K = Cast<ANaveKamikase>(DestroyedActor))
        ListaNavesKamikase.Remove(K);
    else if (ANave_CMN* C = Cast<ANave_CMN>(DestroyedActor))
        ListaNavesCMN.Remove(C);
    else if (ARobotFrancotirador* F = Cast<ARobotFrancotirador>(DestroyedActor))
        ListaFrancotiradores.Remove(F);
    else if (ARobot_Medico* M = Cast<ARobot_Medico>(DestroyedActor))
        ListaMedicos.Remove(M);
    else if (ARobot_RZ* RZ = Cast<ARobot_RZ>(DestroyedActor))
        ListaRobotsRZ.Remove(RZ);
    else if (ARobot_RZ_D* RZD = Cast<ARobot_RZ_D>(DestroyedActor))
        ListaRobotsRZD.Remove(RZD);
    else if (ABossEstatico* B = Cast<ABossEstatico>(DestroyedActor))
        ListaBossEstaticos.Remove(B);
    else if (AFabricaInvulnerable* FInv = Cast<AFabricaInvulnerable>(DestroyedActor))
        ListaFabricas.Remove(FInv);
    VerificarCondicionVictoria();
}

void AGalagaModificadoMacGameMode::VerificarCondicionVictoria()
{
    int32 TotalEnemigosVivos =
        ListaNavesComando.Num() +
        ListaTorretas.Num() +
        ListaRobotsLider.Num() +
        ListaNavesLider.Num() +
        ListaNavesKamikase.Num() +
        ListaNavesCMN.Num() +
        ListaFrancotiradores.Num() +
        ListaMedicos.Num() +
        ListaRobotsRZ.Num() +
        ListaRobotsRZD.Num() +
        ListaBossEstaticos.Num();

    if (TotalEnemigosVivos == 0)
    {
        GetWorldTimerManager().ClearTimer(TimerHandle_Reloj);

        FTimerHandle HandleMudar;
        GetWorldTimerManager().SetTimer(HandleMudar, [this]()
            {
                if (GetGameInstance())
                {
                    UGalagaGameInstance* GI = Cast<UGalagaGameInstance>(GetGameInstance());
                    if (GI)
                    {
                        int32 SiguienteNivel = GI->SelectedLevelIndex + 1;
                        if (SiguienteNivel < 15)
                        {
                            GI->SetSelectedLevel(SiguienteNivel);
                            GI->LaunchGame();
                        }
                        else
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("¡ENHORABUENA! HAS PASADO TODA LA CAMPAÑA"));
                            GI->ReturnToMainMenu();
                        }
                    }
                }
            }, 3.0f, false);
    }
}

// ========== MÚSICA ==========
void AGalagaModificadoMacGameMode::IniciarMusica()
{
    if (!MusicPlayer || !GerenteDeNiveles) return;

    int32 TipoAmbiente = GerenteDeNiveles->ConfiguracionActual.TipoAmbiente;
    USoundBase* MusicaSeleccionada = nullptr;

    if (NivelAIniciar == 12)
    {
        MusicaSeleccionada = MusicaJefeInvulnerable;
    }
    else
    {
        USoundBase** Encontrada = MusicasAmbiente.Find(TipoAmbiente);
        if (Encontrada)
        {
            MusicaSeleccionada = *Encontrada;
        }
    }

    if (MusicaSeleccionada)
    {
        MusicPlayer->SetSound(MusicaSeleccionada);
        MusicPlayer->FadeIn(1.5f);
        MusicPlayer->Play();
    }
}

void AGalagaModificadoMacGameMode::DetenerMusica()
{
    if (MusicPlayer && MusicPlayer->IsPlaying())
    {
        MusicPlayer->FadeOut(0.8f, 0.0f);
    }
}

void AGalagaModificadoMacGameMode::CambiarMusicaJefe(int32 Fase)
{
    if (!MusicPlayer) return;

    USoundBase* NuevaMusica = nullptr;
    if (Fase == 0)
    {
        NuevaMusica = MusicaJefeInvulnerable;
    }
    else if (Fase == 1)
    {
        NuevaMusica = MusicaJefeHeroica;
    }

    if (NuevaMusica && MusicPlayer->Sound != NuevaMusica)
    {
        MusicPlayer->FadeOut(0.5f, 0.0f);
        MusicPlayer->SetSound(NuevaMusica);
        MusicPlayer->FadeIn(0.5f);
        MusicPlayer->Play();
    }
}