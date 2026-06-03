#include "FacadeGeneradorNiveles.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Incluimos todos tus escenarios
#include "EscenarioEspacio.h"
#include "EscenarioCiudad.h"
#include "EscenarioAtmosfera.h"

// Incluimos todas tus fábricas
#include "ObstaculoEspacioFactory.h"
#include "ObstaculoCiudadDestruidaFactory.h"
#include "ObstaculoAtmosferaFactory.h"

void UFacadeGeneradorNiveles::Inicializar(UWorld* WorldContext)
{
	MundoActual = WorldContext;
	EscenarioActivo = nullptr;
	FabricaActiva = nullptr;
}

void UFacadeGeneradorNiveles::DestruirNivelAnterior()
{
	// 1. Destruimos el piso, paredes y la fábrica
	if (EscenarioActivo) EscenarioActivo->Destroy();
	if (FabricaActiva) FabricaActiva->Destroy();

	// 2. Buscamos TODOS los obstáculos huérfanos que quedaron en el mapa
	if (MundoActual)
	{
		TArray<AActor*> ObstaculosViejos;
		// Busca cualquier cosa que herede de AObstaculoDestruido
		UGameplayStatics::GetAllActorsOfClass(MundoActual, AObstaculoDestruido::StaticClass(), ObstaculosViejos);

		// 3. Los destruimos uno por uno
		for (AActor* Obstaculo : ObstaculosViejos)
		{
			if (Obstaculo)
			{
				Obstaculo->Destroy();
			}
		}
	}
}

void UFacadeGeneradorNiveles::CargarNivelEspacio()
{
	if (!MundoActual) return;
	DestruirNivelAnterior();

	// 1. Creamos la fábrica correspondiente
	FabricaActiva = MundoActual->SpawnActor<AObstaculoEspacioFactory>(AObstaculoEspacioFactory::StaticClass());

	// 2. Creamos el Escenario EN PAUSA (Deferred)
	EscenarioActivo = MundoActual->SpawnActorDeferred<AEscenarioEspacio>(AEscenarioEspacio::StaticClass(), FTransform::Identity);

	if (EscenarioActivo)
	{
		// 3. Inyectamos la dependencia (El Builder interno)
		EscenarioActivo->FabricaObstaculos = FabricaActiva;

		// 4. ¡Despausamos! Ahora sí se ejecutará su BeginPlay con la fábrica lista
		UGameplayStatics::FinishSpawningActor(EscenarioActivo, FTransform::Identity);

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("Fachada: Nivel Espacial Cargado"));
	}
}

void UFacadeGeneradorNiveles::CargarNivelCiudad()
{
	if (!MundoActual) return;
	DestruirNivelAnterior();

	FabricaActiva = MundoActual->SpawnActor<AObstaculoCiudadDestruidaFactory>(AObstaculoCiudadDestruidaFactory::StaticClass());
	EscenarioActivo = MundoActual->SpawnActorDeferred<AEscenarioCiudad>(AEscenarioCiudad::StaticClass(), FTransform::Identity);

	if (EscenarioActivo)
	{
		EscenarioActivo->FabricaObstaculos = FabricaActiva;
		UGameplayStatics::FinishSpawningActor(EscenarioActivo, FTransform::Identity);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("Fachada: Nivel Ciudad Cargado"));
	}
}

void UFacadeGeneradorNiveles::CargarNivelAtmosfera()
{
	if (!MundoActual) return;
	DestruirNivelAnterior();

	FabricaActiva = MundoActual->SpawnActor<AObstaculoAtmosferaFactory>(AObstaculoAtmosferaFactory::StaticClass());
	EscenarioActivo = MundoActual->SpawnActorDeferred<AEscenarioAtmosfera>(AEscenarioAtmosfera::StaticClass(), FTransform::Identity);

	if (EscenarioActivo)
	{
		EscenarioActivo->FabricaObstaculos = FabricaActiva;
		UGameplayStatics::FinishSpawningActor(EscenarioActivo, FTransform::Identity);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("Fachada: Nivel Atmosfera Cargado"));
	}
}

void UFacadeGeneradorNiveles::CargarNivelMadreNodriza()
{
	// Aquí pondrás la lógica cuando crees AEscenarioMadreNodriza
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Fachada: Nivel Madre Nodriza en construccion..."));
}