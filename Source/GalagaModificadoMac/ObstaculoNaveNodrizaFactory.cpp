// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoNaveNodrizaFactory.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

AObstaculoNaveNodrizaFactory::AObstaculoNaveNodrizaFactory() 
{
	// Aquí cargas tus mallas de la nave nodriza. 
	// Reemplaza estas rutas de ejemplo por las carpetas reales de tus modelos 3D
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshPasilloAsset(TEXT("StaticMesh'/Game/Modelos/navenodriza/ayudas/torreMoba_Cubo_000.torreMoba_Cubo_000'"));
	if (MeshPasilloAsset.Succeeded())
	{
		MallasPasillos.Add(MeshPasilloAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshTorretaAsset(TEXT("StaticMesh'/Game/Modelos/navenodriza/Obstaculos/FuturePillarsBaked.FuturePillarsBaked'"));
	if (MeshTorretaAsset.Succeeded())
	{
		MallasTorretas.Add(MeshTorretaAsset.Object);
	}
}

AObstaculoDestruido* AObstaculoNaveNodrizaFactory::CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion)
{
	return CrearObstaculoEspecifico(Mundo, Posicion, Rotacion, TEXT("Pasillo"));
}

AObstaculoDestruido* AObstaculoNaveNodrizaFactory::CrearObstaculoEspecifico(UWorld* Mundo, FVector Posicion, FRotator Rotacion, FString Tipo)
{
	if (!Mundo) return nullptr;

	UStaticMesh* MallaElegida = nullptr;

	if (Tipo.Equals(TEXT("Pasillo")) && MallasPasillos.Num() > 0)
	{
		MallaElegida = MallasPasillos[FMath::RandRange(0, MallasPasillos.Num() - 1)];
	}
	else if (Tipo.Equals(TEXT("Torreta")) && MallasTorretas.Num() > 0)
	{
		MallaElegida = MallasTorretas[FMath::RandRange(0, MallasTorretas.Num() - 1)];
	}
	else if (Tipo.Equals(TEXT("Compuerta")) && MallasCompuertas.Num() > 0)
	{
		MallaElegida = MallasCompuertas[FMath::RandRange(0, MallasCompuertas.Num() - 1)];
	}

	if (MallaElegida)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AObstaculoDestruido* NuevoObstaculo = Mundo->SpawnActor<AObstaculoDestruido>(
			Posicion, Rotacion, SpawnParams
			);

		if (NuevoObstaculo)
		{
			UStaticMeshComponent* MeshComp = NuevoObstaculo->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComp)
			{
				MeshComp->SetStaticMesh(MallaElegida);

				if (Tipo.Equals(TEXT("Pasillo")))
				{
					float EscalaPasillo = FMath::RandRange(1.5f, 2.5f);
					MeshComp->SetWorldScale3D(FVector(EscalaPasillo, EscalaPasillo, EscalaPasillo));
				}
				else if (Tipo.Equals(TEXT("Torreta")))
				{
					MeshComp->SetWorldScale3D(FVector(10.0f, 10.0f, 800.0f));
				}
				else if (Tipo.Equals(TEXT("Compuerta")))
				{
					MeshComp->SetWorldScale3D(FVector(25.0f, 25.0f, 100.0f));
				}

				return NuevoObstaculo;
			}
		}
	}

	return nullptr;
}