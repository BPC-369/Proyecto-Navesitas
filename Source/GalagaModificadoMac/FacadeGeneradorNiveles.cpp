#include "FacadeGeneradorNiveles.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "EscenarioDirector.h"
#include "EscenarioBase.h"

// Builders y Escenarios 
#include "EspacioEscenarioBuilder.h"
#include "CiudadEscenarioBuilder.h"
#include "AtmosferaEscenarioBuilder.h"
#include "NaveNodrizaEscenarioBuilder.h"

#include "EscenarioEspacio.h"
#include "EscenarioCiudad.h"
#include "EscenarioAtmosfera.h"
#include "EscenarioNaveNodriza.h"

#include "ObstaculoDestruido.h" 
#include "GalagaModificadoMacGameMode.h"

void UFacadeGeneradorNiveles::Inicializar(UWorld* WorldContext)
{
	MundoActual = WorldContext;
	EscenarioActivo = nullptr;
	InicializarCampana();
}

void UFacadeGeneradorNiveles::InicializarCampana()
{
	CampanaNiveles.Empty();

	// FConfiguracionNivel{ TipoAmbiente, ObsA, ObsB, Comando, Torreta, RobotLider, NaveLider, Kamikase, CMN, Francotirador }

	
	CampanaNiveles.Add(FConfiguracionNivel{ 1, 1000, 0,   5, 0, 0, 0, 0, 0, 0 });
	// Nivel 2: Alerta Kamikase (80 meteoritos, 6 naves comando, 3 kamikases veloces)
	CampanaNiveles.Add(FConfiguracionNivel{ 1, 80, 0,   6, 0, 0, 0, 3, 2, 0 });
	// Nivel 3: El Almirante Estelar (100 meteoritos, 8 comandos, 4 naves CMN, 1 Nave Líder como jefe)
	CampanaNiveles.Add(FConfiguracionNivel{ 1, 100, 0,  8, 0, 0, 1, 2, 4, 0 });


	// Nivel 4: Peligro en el suelo (60 edificios, 20 rocas, 6 Torretas terrestres, 3 Francotiradores )
	CampanaNiveles.Add(FConfiguracionNivel{ 2, 60, 20,  0, 6, 0, 0, 0, 0, 3 });
	// Nivel 5: Invasión Urbana (120 edificios, 40 rocas, 4 comandos volando bajo, 6 torretas, 4 kamikases )
	CampanaNiveles.Add(FConfiguracionNivel{ 2, 120, 40, 4, 6, 0, 0, 4, 0, 4 });
	// Nivel 6: Infierno de Asfalto (200 edificios, 60 rocas, 8 torretas, 6 francotiradores, 8 kamikases)
	CampanaNiveles.Add(FConfiguracionNivel{ 2, 200, 60, 0, 8, 0, 0, 8, 2, 6 });

	
	// MUNDO 3:
	
	CampanaNiveles.Add(FConfiguracionNivel{ 3, 80, 0,   10, 0, 0, 0, 4, 4, 0 });  // Nivel 7
	CampanaNiveles.Add(FConfiguracionNivel{ 3, 150, 0,  12, 0, 0, 1, 6, 6, 0 });  // Nivel 8
	CampanaNiveles.Add(FConfiguracionNivel{ 3, 300, 0,  15, 0, 0, 2, 8, 8, 0 });  // Nivel 9

//MUNDO 4
	CampanaNiveles.Add(FConfiguracionNivel{ 4, 30, 5,   8,  6,  1, 0, 4, 0, 0 });  // Nivel 10: Custodiado por 1 Robot Líder
	CampanaNiveles.Add(FConfiguracionNivel{ 4, 60, 12,  10, 10, 2, 0, 6, 0, 4 });  // Nivel 11
	CampanaNiveles.Add(FConfiguracionNivel{ 4, 90, 20,  12, 14, 3, 0, 8, 4, 6 });  // Nivel 12


	// CLÍMAX FINAL: RESISTENCIA EXTREMA (Jefes mixtos)

	CampanaNiveles.Add(FConfiguracionNivel{ 1, 400, 0,  20, 0,  0, 4, 15, 10, 0 });  // Nivel 13
	CampanaNiveles.Add(FConfiguracionNivel{ 2, 250, 90, 10, 16, 0, 2, 12, 8,  10 }); // Nivel 14
	CampanaNiveles.Add(FConfiguracionNivel{ 4, 150, 35, 15, 20, 5, 3, 20, 12, 8 });  // Nivel 15: El Núcleo
}

void UFacadeGeneradorNiveles::CargarNivelPorIndice(int32 IndiceNivel)
{
	if (!MundoActual || !CampanaNiveles.IsValidIndex(IndiceNivel))
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
	case 1:
		BuilderElegido = MundoActual->SpawnActor<AEspacioEscenarioBuilder>(AEspacioEscenarioBuilder::StaticClass(), SpawnParams);
		ClaseEscenarioProducto = AEscenarioEspacio::StaticClass();
		break;
	case 2:
		BuilderElegido = MundoActual->SpawnActor<ACiudadEscenarioBuilder>(ACiudadEscenarioBuilder::StaticClass(), SpawnParams);
		ClaseEscenarioProducto = AEscenarioCiudad::StaticClass();
		break;
	case 3:
		BuilderElegido = MundoActual->SpawnActor<AAtmosferaEscenarioBuilder>(AAtmosferaEscenarioBuilder::StaticClass(), SpawnParams);
		ClaseEscenarioProducto = AEscenarioAtmosfera::StaticClass();
		break;
	case 4:
		BuilderElegido = MundoActual->SpawnActor<ANaveNodrizaEscenarioBuilder>(ANaveNodrizaEscenarioBuilder::StaticClass(), SpawnParams);
		ClaseEscenarioProducto = AEscenarioNaveNodriza::StaticClass();
		break;
	}

	if (Director && BuilderElegido && ClaseEscenarioProducto)
	{
		EscenarioActivo = Director->ConstruirEscenario(BuilderElegido, ClaseEscenarioProducto);

		if (EscenarioActivo)
		{
			// 1. Inyectamos los datos PRIMERO
			EscenarioActivo->CantidadObstaculosA = DatosNivel.CantidadObstaculosA;
			EscenarioActivo->CantidadObstaculosB = DatosNivel.CantidadObstaculosB;

			// 2. ¡EL GATILLO MANUAL! Le decimos al escenario que ya puede spawnear
			// Hacemos un Cast seguro al escenario de espacio
			AEscenarioEspacio* EscenarioMundo1 = Cast<AEscenarioEspacio>(EscenarioActivo);
			if (EscenarioMundo1)
			{
				EscenarioMundo1->GenerarEntornoEspacial(); 
			}

			// (Aquí puedes agregar luego los casts para la Ciudad o la Atmósfera si usan métodos diferentes)

			// 3. Pasamos los datos al GameMode
			AGalagaModificadoMacGameMode* MyGameMode = Cast<AGalagaModificadoMacGameMode>(MundoActual->GetAuthGameMode());
			if (MyGameMode)
			{
				MyGameMode->CantNaveComando = DatosNivel.CantNaveComando;
				MyGameMode->CantTorreta = DatosNivel.CantTorreta;
				MyGameMode->CantRobotLider = DatosNivel.CantRobotLider;
				MyGameMode->CantNaveLider = DatosNivel.CantNaveLider;
				MyGameMode->CantNaveKamikase = DatosNivel.CantNaveKamikase;
				MyGameMode->CantNaveCMN = DatosNivel.CantNaveCMN;
				MyGameMode->CantFrancotirador = DatosNivel.CantRobotFrancotirador;
				MyGameMode->AmbienteActual = DatosNivel.TipoAmbiente;
			}
		}

		BuilderElegido->Destroy();
		Director->Destroy();

		FString MensajeInfo = FString::Printf(TEXT("Fachada: Cargado Nivel %d (Ambiente %d)"), (IndiceNivel + 1), DatosNivel.TipoAmbiente);
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, MensajeInfo);
	}
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