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

	// Llenamos el mapa de la campaña
	InicializarCampana();
}

void UFacadeGeneradorNiveles::InicializarCampana()
{
	CampanaNiveles.Empty();
	FConfiguracionNivel Nivel;

	Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 3000; Nivel.CantidadObstaculosB = 3000;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 5); // ID 5 (Kamikaze) -> Cantidad: 5
	Nivel.EnemigosPorGenerar.Add(8, 5); // ID 1 (Comando) -> Cantidad: 2
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 10); // 10 Kamikazes
	Nivel.EnemigosPorGenerar.Add(1, 4);  // 4 Naves CMN
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(2, 6);  // 6 Torretas (Suelo)
	Nivel.EnemigosPorGenerar.Add(7, 3);  // 3 Francotiradores (Suelo)
	Nivel.EnemigosPorGenerar.Add(4, 2);  // 2 Naves Líder (Aire)
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 5); // ID 5 (Kamikaze) -> Cantidad: 5
	Nivel.EnemigosPorGenerar.Add(1, 2); // ID 1 (Comando) -> Cantidad: 2
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 10); // 10 Kamikazes
	Nivel.EnemigosPorGenerar.Add(6, 4);  // 4 Naves CMN
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 2; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(2, 6);  // 6 Torretas (Suelo)
	Nivel.EnemigosPorGenerar.Add(7, 3);  // 3 Francotiradores (Suelo)
	Nivel.EnemigosPorGenerar.Add(4, 2);  // 2 Naves Líder (Aire)
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 5); // ID 5 (Kamikaze) -> Cantidad: 5
	Nivel.EnemigosPorGenerar.Add(1, 2); // ID 1 (Comando) -> Cantidad: 2
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 10); // 10 Kamikazes
	Nivel.EnemigosPorGenerar.Add(6, 4);  // 4 Naves CMN
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 3; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(2, 6);  // 6 Torretas (Suelo)
	Nivel.EnemigosPorGenerar.Add(7, 3);  // 3 Francotiradores (Suelo)
	Nivel.EnemigosPorGenerar.Add(4, 2);  // 2 Naves Líder (Aire)
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 5); // ID 5 (Kamikaze) -> Cantidad: 5
	Nivel.EnemigosPorGenerar.Add(1, 2); // ID 1 (Comando) -> Cantidad: 2
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 10); // 10 Kamikazes
	Nivel.EnemigosPorGenerar.Add(6, 4);  // 4 Naves CMN
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(2, 6);  // 6 Torretas (Suelo)
	Nivel.EnemigosPorGenerar.Add(7, 3);  // 3 Francotiradores (Suelo)
	Nivel.EnemigosPorGenerar.Add(4, 2);  // 2 Naves Líder (Aire)
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 3; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 5); // ID 5 (Kamikaze) -> Cantidad: 5
	Nivel.EnemigosPorGenerar.Add(1, 2); // ID 1 (Comando) -> Cantidad: 2
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 1; Nivel.CantidadObstaculosA = 10; Nivel.CantidadObstaculosB = 0;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(5, 10); // 10 Kamikazes
	Nivel.EnemigosPorGenerar.Add(6, 4);  // 4 Naves CMN
	CampanaNiveles.Add(Nivel);

	Nivel.TipoAmbiente = 4; Nivel.CantidadObstaculosA = 60; Nivel.CantidadObstaculosB = 20;
	Nivel.EnemigosPorGenerar.Empty();
	Nivel.EnemigosPorGenerar.Add(2, 6);  // 6 Torretas (Suelo)
	Nivel.EnemigosPorGenerar.Add(7, 3);  // 3 Francotiradores (Suelo)
	Nivel.EnemigosPorGenerar.Add(4, 2);  // 2 Naves Líder (Aire)
	CampanaNiveles.Add(Nivel);
}

void UFacadeGeneradorNiveles::DestruirNivelAnterior()
{
	// 1. Mandamos a destruir el escenario activo anterior
	if (EscenarioActivo)
	{
		// Si el escenario tenía una fábrica adentro, la destruimos desde el padre
		if (EscenarioActivo->FabricaObstaculos)
		{
			EscenarioActivo->FabricaObstaculos->Destroy();
		}
		EscenarioActivo->Destroy();
		EscenarioActivo = nullptr;
	}

	// 2. Limpieza profunda de los actores que quedaron sueltos en el mapa
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

	// Validamos que el nivel exista en el rango de nuestra campaña (0 a 14)
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
	case 1: //Espacio
	{
		AEspacioEscenarioBuilder* EspacioBuilder = MundoActual->SpawnActor<AEspacioEscenarioBuilder>(AEspacioEscenarioBuilder::StaticClass(), SpawnParams);
		if (EspacioBuilder)
		{
			BuilderElegido = EspacioBuilder;
		}
		ClaseEscenarioProducto = AEscenarioEspacio::StaticClass();
		break;
	}

	case 2: // CIUDAD
	{
		ACiudadEscenarioBuilder* CiudadBuilder = MundoActual->SpawnActor<ACiudadEscenarioBuilder>(ACiudadEscenarioBuilder::StaticClass(), SpawnParams);
		if (CiudadBuilder)
		{
			BuilderElegido = CiudadBuilder;
		}
		ClaseEscenarioProducto = AEscenarioCiudad::StaticClass();
		break;
	}

	case 3: // ATMÓSFERA
	{
		AAtmosferaEscenarioBuilder* AtmosferaBuilder = MundoActual->SpawnActor<AAtmosferaEscenarioBuilder>(AAtmosferaEscenarioBuilder::StaticClass(), SpawnParams);
		if (AtmosferaBuilder)
		{
			BuilderElegido = AtmosferaBuilder;
		}
		ClaseEscenarioProducto = AEscenarioAtmosfera::StaticClass();
		break;
	}

	case 4: // NAVE NODRIZA 
	{
		ANaveNodrizaEscenarioBuilder* NodrizaBuilder = MundoActual->SpawnActor<ANaveNodrizaEscenarioBuilder>(ANaveNodrizaEscenarioBuilder::StaticClass(), SpawnParams);
		if (NodrizaBuilder)
		{
			BuilderElegido = NodrizaBuilder;
		}
		ClaseEscenarioProducto = AEscenarioNaveNodriza::StaticClass();
		break;
	}
	}

	if (Director && BuilderElegido && ClaseEscenarioProducto)
	{
		// 1. PASAMOS LOS DATOS AL BUILDER (100 meteoritos, 50 nubes, etc.)
		BuilderElegido->SetDatosNivel(DatosNivel.CantidadObstaculosA, DatosNivel.CantidadObstaculosB);

		// 2. EL DIRECTOR CONSTRUYE Y ASIGNA LA FÁBRICA
		EscenarioActivo = Director->ConstruirEscenario(BuilderElegido, ClaseEscenarioProducto);

		// 3. LA ORDEN DE SPAWN (Ahora sí la fábrica existe)
		if (EscenarioActivo)
		{
			EscenarioActivo->GenerarObstaculosProcedurales();
		}

		// Limpieza de memoria
		BuilderElegido->Destroy();
		Director->Destroy();

		FString MensajeInfo = FString::Printf(TEXT("Fachada: Cargado Nivel %d"), (IndiceNivel + 1));
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, MensajeInfo);
	}
}