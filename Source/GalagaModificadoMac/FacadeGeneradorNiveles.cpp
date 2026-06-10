#include "FacadeGeneradorNiveles.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GestorBonificaciones.h"

#include "EscenarioDirector.h"
#include "EscenarioBase.h"

#include "EspacioEscenarioBuilder.h"
#include "CiudadEscenarioBuilder.h"
#include "AtmosferaEscenarioBuilder.h"
#include "NaveNodrizaEscenarioBuilder.h"
#include "NaveNodrizaGefeBuilder.h"

#include "EscenarioEspacio.h"
#include "EscenarioCiudad.h"
#include "EscenarioAtmosfera.h"
#include "EscenarioNaveNodriza.h"

#include "ObstaculoDestruido.h"

void UFacadeGeneradorNiveles::Inicializar(UWorld* WorldContext)
{
    MundoActual = WorldContext;
    EscenarioActivo = nullptr;
    InicializarCampana();
}

void UFacadeGeneradorNiveles::InicializarCampana()
{
    CampanaNiveles.Empty();
    FConfiguracionNivel Nivel;

    // ========== ESPACIO (bioma 1) – índices 0,1,2 =================================
    Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 5000; Nivel.CantidadObstaculosB = 5000;
    Nivel.TiempoLimite = 300;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5); // Kamikaze
    Nivel.EnemigosPorGenerar.Add(1, 2); // Comando
    Nivel.EnemigosPorGenerar.Add(4, 1); // Nave Líder
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 500; Nivel.CantidadObstaculosB = 120;
    Nivel.TiempoLimite = 300;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(1, 4);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
    Nivel.TiempoLimite = 300;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 6);
    Nivel.EnemigosPorGenerar.Add(1, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    CampanaNiveles.Add(Nivel);

    // ========== ATMÓSFERA (bioma 2) – índices 3,4,5 ==============================
    Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 550; Nivel.CantidadObstaculosB = 50;
    Nivel.TiempoLimite = 300;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(4, 3);
    Nivel.EnemigosPorGenerar.Add(1, 3);
    Nivel.EnemigosPorGenerar.Add(6, 3);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 550; Nivel.CantidadObstaculosB = 15;
    Nivel.TiempoLimite = 300;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(1, 3);
    Nivel.EnemigosPorGenerar.Add(6, 4);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 550; Nivel.CantidadObstaculosB = 15;
    Nivel.TiempoLimite = 300;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(1, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    CampanaNiveles.Add(Nivel);

    // ========== CIUDAD OMEGA (bioma 3) – índices 6,7,8 ===========================
    Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 120; Nivel.CantidadObstaculosB = 120;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(2, 10);
    Nivel.EnemigosPorGenerar.Add(7, 0);
    Nivel.EnemigosPorGenerar.Add(9, 0);
    Nivel.EnemigosPorGenerar.Add(10, 0);
    Nivel.EnemigosPorGenerar.Add(11, 0);
    Nivel.EnemigosPorGenerar.Add(12, 1); // 1 cuartel terrestre
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 0; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(6, 4);
    Nivel.EnemigosPorGenerar.Add(12, 2); // 2 cuarteles
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 0; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    Nivel.EnemigosPorGenerar.Add(12, 1); // 1 cuartel
    CampanaNiveles.Add(Nivel);

    // ========== NAVE NODRIZA (bioma 4) – índices 9,10,11 =========================
    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 0; Nivel.CantidadObstaculosB = 50;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(1, 5);
    Nivel.EnemigosPorGenerar.Add(12, 1); // 1 cuartel
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 0; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(1, 1);
    Nivel.EnemigosPorGenerar.Add(12, 2); // 2 cuarteles
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 0; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    Nivel.EnemigosPorGenerar.Add(12, 1); // 1 cuartel
    CampanaNiveles.Add(Nivel);

    // ========== JEFE FINAL (bioma 5) – índice 12 (nivel 13) ======================
    Nivel.TipoAmbiente = 5; Nivel.CantidadObstaculosA = 50; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(8, 1);  // Boss
    CampanaNiveles.Add(Nivel);

    // ========== NIVELES POST‑JEFE – índices 13 y 14 ==============================
    // Nivel 14 (índice 13) – Escape encarnizado (nave nodriza, todos enemigos)
    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 0; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(1, 4);  // naves comando
    Nivel.EnemigosPorGenerar.Add(5, 8);  // kamikazes
    Nivel.EnemigosPorGenerar.Add(6, 6);  // CMN
    Nivel.EnemigosPorGenerar.Add(12, 3); // 3 cuarteles
    CampanaNiveles.Add(Nivel);

    // Nivel 15 (índice 14) – Batalla solo naves
    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 0; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(1, 6);
    Nivel.EnemigosPorGenerar.Add(4, 4);
    Nivel.EnemigosPorGenerar.Add(5, 12);
    Nivel.EnemigosPorGenerar.Add(6, 10);
    // Sin cuarteles ni robots terrestres
    CampanaNiveles.Add(Nivel);
}

void UFacadeGeneradorNiveles::DestruirNivelAnterior()
{
    if (EscenarioActivo)
    {
        if (EscenarioActivo->FabricaObstaculos)
        {
            EscenarioActivo->FabricaObstaculos->Destroy();
        }
        EscenarioActivo->Destroy();
        EscenarioActivo = nullptr;
    }

    if (MundoActual)
    {
        TArray<AActor*> GestoresViejos;
        UGameplayStatics::GetAllActorsOfClass(MundoActual, AGestorBonificaciones::StaticClass(), GestoresViejos);
        for (AActor* Gestor : GestoresViejos)
        {
            if (Gestor) Gestor->Destroy();
        }
        TArray<AActor*> ObstaculosHuerfanos;
        UGameplayStatics::GetAllActorsOfClass(MundoActual, AObstaculoDestruido::StaticClass(), ObstaculosHuerfanos);
        for (AActor* Obstaculo : ObstaculosHuerfanos)
        {
            if (Obstaculo) Obstaculo->Destroy();
        }
    }
}

void UFacadeGeneradorNiveles::CargarNivelPorIndice(int32 IndiceNivel)
{
    if (!MundoActual) return;

    if (!CampanaNiveles.IsValidIndex(IndiceNivel))
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Campaña Completada con éxito!"));
        return;
    }

    DestruirNivelAnterior();

    ConfiguracionActual = CampanaNiveles[IndiceNivel];
    FConfiguracionNivel DatosNivel = CampanaNiveles[IndiceNivel];

    FActorSpawnParameters SpawnParams;

    AEscenarioDirector* Director = MundoActual->SpawnActor<AEscenarioDirector>(AEscenarioDirector::StaticClass(), SpawnParams);

    AEscenarioBuilder* BuilderElegido = nullptr;
    UClass* ClaseEscenarioProducto = nullptr;

    switch (DatosNivel.TipoAmbiente)
    {
    case 1:
    {
        AEspacioEscenarioBuilder* EspacioBuilder = MundoActual->SpawnActor<AEspacioEscenarioBuilder>(AEspacioEscenarioBuilder::StaticClass(), SpawnParams);
        if (EspacioBuilder) BuilderElegido = EspacioBuilder;
        ClaseEscenarioProducto = AEscenarioEspacio::StaticClass();
        break;
    }
    case 2:
    {
        AAtmosferaEscenarioBuilder* AtmosferaBuilder = MundoActual->SpawnActor<AAtmosferaEscenarioBuilder>(AAtmosferaEscenarioBuilder::StaticClass(), SpawnParams);
        if (AtmosferaBuilder) BuilderElegido = AtmosferaBuilder;
        ClaseEscenarioProducto = AEscenarioAtmosfera::StaticClass();
        break;
    }
    case 3:
    {
        ACiudadEscenarioBuilder* CiudadBuilder = MundoActual->SpawnActor<ACiudadEscenarioBuilder>(ACiudadEscenarioBuilder::StaticClass(), SpawnParams);
        if (CiudadBuilder) BuilderElegido = CiudadBuilder;
        ClaseEscenarioProducto = AEscenarioCiudad::StaticClass();
        break;
    }
    case 4:
    {
        ANaveNodrizaEscenarioBuilder* NodrizaBuilder = MundoActual->SpawnActor<ANaveNodrizaEscenarioBuilder>(ANaveNodrizaEscenarioBuilder::StaticClass(), SpawnParams);
        if (NodrizaBuilder) BuilderElegido = NodrizaBuilder;
        ClaseEscenarioProducto = AEscenarioNaveNodriza::StaticClass();
        break;
    }
    case 5:
    {
        ANaveNodrizaGefeBuilder* GefeBuilder = MundoActual->SpawnActor<ANaveNodrizaGefeBuilder>(ANaveNodrizaGefeBuilder::StaticClass(), SpawnParams);
        if (GefeBuilder) BuilderElegido = GefeBuilder;
        ClaseEscenarioProducto = AEscenarioNaveNodriza::StaticClass();
        break;
    }
    }

    if (Director && BuilderElegido && ClaseEscenarioProducto)
    {
        BuilderElegido->SetDatosNivel(DatosNivel.CantidadObstaculosA, DatosNivel.CantidadObstaculosB);
        EscenarioActivo = Director->ConstruirEscenario(BuilderElegido, ClaseEscenarioProducto);

        if (EscenarioActivo)
        {
            EscenarioActivo->GenerarObstaculosProcedurales();
        }
        if (MundoActual)
        {
            FActorSpawnParameters SpawnBonifParams;
            SpawnBonifParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            MundoActual->SpawnActor<AGestorBonificaciones>(
                AGestorBonificaciones::StaticClass(),
                FVector::ZeroVector,
                FRotator::ZeroRotator,
                SpawnBonifParams
                );
        }
        BuilderElegido->Destroy();
        Director->Destroy();

        FString MensajeInfo = FString::Printf(TEXT("Fachada: Cargado Nivel %d"), (IndiceNivel + 1));
        GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, MensajeInfo);
    }
}

void UFacadeGeneradorNiveles::AplicarDificultad(const FString& Difficulty)
{
    float MultiplicadorCantidad = 1.0f;
    MultiplicadorDificultad = 1.0f;

    if (Difficulty == TEXT("Easy"))
    {
        MultiplicadorCantidad = 0.7f;
        MultiplicadorDificultad = 0.5f;
    }
    else if (Difficulty == TEXT("Hard"))
    {
        MultiplicadorCantidad = 2.0f;
        MultiplicadorDificultad = 3.0f;
    }

    for (auto& Par : ConfiguracionActual.EnemigosPorGenerar)
    {
        int32 NuevaCantidad = FMath::RoundToInt(Par.Value * MultiplicadorCantidad);
        Par.Value = FMath::Max(1, NuevaCantidad);
    }

    UE_LOG(LogTemp, Warning, TEXT("Dificultad aplicada: %s | Multiplicador cantidad: %f | Multiplicador vida/daño: %f"),
        *Difficulty, MultiplicadorCantidad, MultiplicadorDificultad);
}