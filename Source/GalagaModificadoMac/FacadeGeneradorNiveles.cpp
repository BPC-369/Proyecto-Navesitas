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

	// {TipoAmbiente, CantidadA, CantidadB, CantidadEnemigos}

	// Mundo 1: Espacio Profundo
	CampanaNiveles.Add({ 1, 50,  0,  10 }); // Nivel 1
	CampanaNiveles.Add({ 1, 100, 0,  15 }); // Nivel 2
	CampanaNiveles.Add({ 1, 200, 0,  25 }); // Nivel 3

	// Mundo 2: Ciudad Destruida (CantidadA = Edificios, CantidadB = Rocas/Árboles)
	CampanaNiveles.Add({ 2, 60,  20, 15 }); // Nivel 4
	CampanaNiveles.Add({ 2, 120, 40, 25 }); // Nivel 5
	CampanaNiveles.Add({ 2, 200, 60, 40 }); // Nivel 6

	// Mundo 3: Atmósfera
	CampanaNiveles.Add({ 3, 80,  0,  20 }); // Nivel 7
	CampanaNiveles.Add({ 3, 150, 0,  30 }); // Nivel 8
	CampanaNiveles.Add({ 3, 300, 0,  45 }); // Nivel 9

	// Mundo 4: Infiltración Nave Nodriza (CantidadA = Pasillos, CantidadB = Torretas)
	CampanaNiveles.Add({ 4, 30,  5,  25 }); // Nivel 10
	CampanaNiveles.Add({ 4, 60,  12, 35 }); // Nivel 11
	CampanaNiveles.Add({ 4, 90,  20, 50 }); // Nivel 12

	// Clímax Final (Niveles mixtos/extremos de resistencia)
	CampanaNiveles.Add({ 1, 400, 0,  60 }); // Nivel 13: Super Lluvia Meteoritos
	CampanaNiveles.Add({ 2, 250, 90, 70 }); // Nivel 14: Megaciudad infestada
	CampanaNiveles.Add({ 4, 150, 35, 99 }); // Nivel 15: Núcleo de la Nave Nodriza (Jefe Final)
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
		EscenarioActivo = Director->ConstruirEscenario(BuilderElegido, ClaseEscenarioProducto);


		// limpieza de builders
		BuilderElegido->Destroy();
		Director->Destroy();

		FString MensajeInfo = FString::Printf(TEXT("Fachada: Cargado Nivel %d (Ambiente %d)"), (IndiceNivel + 1), DatosNivel.TipoAmbiente);
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, MensajeInfo);
	}
}