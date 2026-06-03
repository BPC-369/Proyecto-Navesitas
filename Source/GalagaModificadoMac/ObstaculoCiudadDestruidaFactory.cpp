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

AObstaculoDestruido* AObstaculoCiudadDestruidaFactory::CrearObstaculoEspecifico(UWorld* Mundo, FVector Posicion, FRotator Rotacion, FString Tipo)
{
	if (!Mundo) return nullptr;

	UStaticMesh* MallaElegida = nullptr;

	// Corregido el uso de 'Tipo' para que coincida exactamente con la firma del .h
	if (Tipo.Equals(TEXT("Edificio")) && MallasEdificios.Num() > 0)
	{
		int32 Indice = FMath::RandRange(0, MallasEdificios.Num() - 1);
		MallaElegida = MallasEdificios[Indice];
	}
	else if (Tipo.Equals(TEXT("Arbol")) && MallasArboles.Num() > 0)
	{
		int32 Indice = FMath::RandRange(0, MallasArboles.Num() - 1);
		MallaElegida = MallasArboles[Indice];
	}
	else if (Tipo.Equals(TEXT("Roca")) && MallasRocas.Num() > 0)
	{
		int32 Indice = FMath::RandRange(0, MallasRocas.Num() - 1);
		MallaElegida = MallasRocas[Indice];
	}

	// Si encontramos la malla correspondiente, spawneamos el objeto
	if (MallaElegida)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AObstaculoDestruido* NuevoObstaculo = Mundo->SpawnActor<AObstaculoDestruido>(
			AObstaculoDestruido::StaticClass(),
			Posicion,
			Rotacion,
			SpawnParams
		);

		if (NuevoObstaculo)
		{
			// SOLUCIÓN AL C2039: Buscamos el componente de malla de forma genérica y segura en Unreal
			UStaticMeshComponent* MeshComp = NuevoObstaculo->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComp)
			{
				MeshComp->SetStaticMesh(MallaElegida);
				return NuevoObstaculo;
			}
		}
	}

	return nullptr;
}
// Agrégalo al final de ObstaculoCiudadDestruidaFactory.cpp
AObstaculoDestruido* AObstaculoCiudadDestruidaFactory::CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion)
{
	// Redirige al método nuevo que creamos para cumplir el contrato de herencia
	return CrearObstaculoEspecifico(Mundo, Posicion, Rotacion, TEXT("Edificio"));
}
