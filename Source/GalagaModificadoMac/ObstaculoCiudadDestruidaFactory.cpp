// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoCiudadDestruidaFactory.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

AObstaculoCiudadDestruidaFactory::AObstaculoCiudadDestruidaFactory()
{
	// Cargar las mallas correspondientes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CuboMesh(TEXT("StaticMesh'/Game/Modelos/Edificios/Edificiodestruido/Meshy_AI_Rustbound_Citadel_0603023016_texture.Meshy_AI_Rustbound_Citadel_0603023016_texture'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EsferaMesh(TEXT("StaticMesh'/Game/Modelos/rocas/rock.rock'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConoMesh(TEXT("StaticMesh'/Game/Modelos/arboles/fbx_game_export_Gum_Tree_Green2.fbx_game_export_Gum_Tree_Green2'"));

	if (CuboMesh.Succeeded())   MallasEdificios.Add(CuboMesh.Object);
	if (EsferaMesh.Succeeded())  MallasRocas.Add(EsferaMesh.Object);
	if (ConoMesh.Succeeded())    MallasArboles.Add(ConoMesh.Object);
}

AObstaculoDestruido* AObstaculoCiudadDestruidaFactory::CrearObstaculoEspecifico(UWorld* Mundo, FVector Posicion, FRotator Rotacion, FString Tipo)
{
	if (!Mundo) return nullptr;

	UStaticMesh* MallaElegida = nullptr;

	// Seleccionamos la malla según el tipo
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

	// Si encontramos la malla, spawneamos el objeto
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
			UStaticMeshComponent* MeshComp = NuevoObstaculo->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComp)
			{
				MeshComp->SetStaticMesh(MallaElegida);


				if (Tipo.Equals(TEXT("Edificio")))
				{
					// Edificios imponentes: 2.5x de ancho y altura aleatoria entre 4x y 7x
					float AlturaEdificio = FMath::RandRange(4.0f, 7.0f);
					MeshComp->SetWorldScale3D(FVector(2.5f, 2.5f, AlturaEdificio));
				}
				else if (Tipo.Equals(TEXT("Arbol")))
				{
					// Arbolitos con escala variada pero natural
					float EscalaArbol = FMath::RandRange(0.8f, 1.6f);
					MeshComp->SetWorldScale3D(FVector(EscalaArbol, EscalaArbol, EscalaArbol));
				}
				else if (Tipo.Equals(TEXT("Roca")))
				{
					// Rocas grandes para bloquear el paso en el suelo
					float EscalaRoca = FMath::RandRange(1.5f, 2.5f);
					MeshComp->SetWorldScale3D(FVector(EscalaRoca, EscalaRoca, EscalaRoca));
				}
				// ============================================================

				return NuevoObstaculo;
			}
		}
	}

	return nullptr;
}

AObstaculoDestruido* AObstaculoCiudadDestruidaFactory::CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion)
{
	// Método heredado por defecto crea un edificio
	return CrearObstaculoEspecifico(Mundo, Posicion, Rotacion, TEXT("Edificio"));
}