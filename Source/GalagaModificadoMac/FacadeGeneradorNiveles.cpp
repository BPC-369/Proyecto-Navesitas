#include "FacadeGeneradorNiveles.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// INCLUDES DE TU ARQUITECTURA BUILDER (Respetando tus patrones)
#include "EscenarioDirector.h"
#include "EscenarioBase.h"

// Tus 4 Builders Concretos
#include "EspacioEscenarioBuilder.h"
#include "CiudadEscenarioBuilder.h"
#include "AtmosferaEscenarioBuilder.h"
#include "NaveNodrizaEscenarioBuilder.h"

// Tus 4 Productos de Escenario
#include "EscenarioEspacio.h"
#include "EscenarioCiudad.h"
#include "EscenarioAtmosfera.h"
#include "EscenarioNaveNodriza.h"

// Clase base de obstáculos para la limpieza
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

    Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 3000; Nivel.CantidadObstaculosB = 3000;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(1, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(6, 4);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(1, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(6, 4);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(1, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(6, 4);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(1, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(6, 4);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 5);
    Nivel.EnemigosPorGenerar.Add(1, 2);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(5, 10);
    Nivel.EnemigosPorGenerar.Add(6, 4);
    CampanaNiveles.Add(Nivel);

    Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
    Nivel.EnemigosPorGenerar.Empty();
    Nivel.EnemigosPorGenerar.Add(2, 6);
    Nivel.EnemigosPorGenerar.Add(7, 3);
    Nivel.EnemigosPorGenerar.Add(4, 2);
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
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("¡Campaña Completada con Éxito!"));
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
        ACiudadEscenarioBuilder* CiudadBuilder = MundoActual->SpawnActor<ACiudadEscenarioBuilder>(ACiudadEscenarioBuilder::StaticClass(), SpawnParams);
        if (CiudadBuilder) BuilderElegido = CiudadBuilder;
        ClaseEscenarioProducto = AEscenarioCiudad::StaticClass();
        break;
    }
    case 3:
    {
        AAtmosferaEscenarioBuilder* AtmosferaBuilder = MundoActual->SpawnActor<AAtmosferaEscenarioBuilder>(AAtmosferaEscenarioBuilder::StaticClass(), SpawnParams);
        if (AtmosferaBuilder) BuilderElegido = AtmosferaBuilder;
        ClaseEscenarioProducto = AEscenarioAtmosfera::StaticClass();
        break;
    }
    case 4:
    {
        ANaveNodrizaEscenarioBuilder* NodrizaBuilder = MundoActual->SpawnActor<ANaveNodrizaEscenarioBuilder>(ANaveNodrizaEscenarioBuilder::StaticClass(), SpawnParams);
        if (NodrizaBuilder) BuilderElegido = NodrizaBuilder;
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
        MultiplicadorCantidad = 1.5f;
        MultiplicadorDificultad = 2.0f;
    }
    // Normal: ambos se quedan en 1.0

    for (auto& Par : ConfiguracionActual.EnemigosPorGenerar)
    {
        int32 NuevaCantidad = FMath::RoundToInt(Par.Value * MultiplicadorCantidad);
        Par.Value = FMath::Max(1, NuevaCantidad);
    }

    UE_LOG(LogTemp, Warning, TEXT("Dificultad aplicada: %s | Multiplicador cantidad: %f | Multiplicador vida/daño: %f"),
        *Difficulty, MultiplicadorCantidad, MultiplicadorDificultad);
}