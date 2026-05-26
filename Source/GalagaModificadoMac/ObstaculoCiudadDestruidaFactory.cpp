// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoCiudadDestruidaFactory.h"
#include "UObject/ConstructorHelpers.h"

AObstaculoCiudadDestruidaFactory::AObstaculoCiudadDestruidaFactory()
{
	// cargar las mallas correspondientes sujeto a cambios
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CuboMesh(TEXT("StaticMesh'/Game/Modelos/Edificios/fbxTower_structure.fbxTower_structure'"));//malla de edificios
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EsferaMesh(TEXT("StaticMesh'/Game/Modelos/rocas/rock.rock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConoMesh(TEXT("StaticMesh'/Game/Modelos/arboles/fbx_game_export_Gum_Tree_Green2.fbx_game_export_Gum_Tree_Green2'"));//mall de losa rbolitos pi

	if (CuboMesh.Succeeded())   MallasEdificios.Add(CuboMesh.Object);
	if (EsferaMesh.Succeeded())  MallasRocas.Add(EsferaMesh.Object);
	if (ConoMesh.Succeeded())    MallasArboles.Add(ConoMesh.Object);
}

AObstaculoDestruido* AObstaculoCiudadDestruidaFactory::CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion)
{
	if (!Mundo) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AObstaculoDestruido* NuevoObstaculo = Mundo->SpawnActor<AObstaculoDestruido>(AObstaculoDestruido::StaticClass(), Posicion, Rotacion, SpawnParams);

	if (NuevoObstaculo)
	{
		// fabrica de que obstaculo spawnear aleatoriamente
		int32 TipoSuerte = FMath::RandRange(1, 3);

		if (TipoSuerte == 1 && MallasEdificios.Num() > 0) // Es un Edificio
		{
			NuevoObstaculo->MallaComponent->SetStaticMesh(MallasEdificios[0]);
	
			float AlturaEdificio = FMath::FRandRange(15.0f, 80.0f); // <-- AQUÍ CONTROLAS QUE SEAN BIEN ALTOS
			NuevoObstaculo->SetActorScale3D(FVector(20.0f, 20.0f, AlturaEdificio));
		}
		else if (TipoSuerte == 2 && MallasRocas.Num() > 0) // Es una Roca/Escombro
		{
			NuevoObstaculo->MallaComponent->SetStaticMesh(MallasRocas[0]);
	
			float EscalaRoca = FMath::FRandRange(1.0f, 2.0f);
			NuevoObstaculo->SetActorScale3D(FVector(EscalaRoca, EscalaRoca, EscalaRoca));
		}
		else if (TipoSuerte == 3 && MallasArboles.Num() > 0) // Es un Árbol quemado/destruido
		{
			NuevoObstaculo->MallaComponent->SetStaticMesh(MallasArboles[0]);;
	
			float EscalaArbol = FMath::FRandRange(0.01f, 0.01f);
			NuevoObstaculo->SetActorScale3D(FVector(0.01f, 0.01f, EscalaArbol));
		}
	}

	return NuevoObstaculo;
}
