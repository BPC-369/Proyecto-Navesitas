// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioEspacio.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "ObstaculoEspacioFactory.h" 
#include "Engine/World.h"

AEscenarioEspacio::AEscenarioEspacio()
{
	AnchoX = 100000.0f;
	LargoY = 100000.0f;
	AltoZ = 10000.0f;

	// malla del domo 
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialEspacioAsset(TEXT("Material'/Game/Modelos/MEspacioProfundo.MEspacioProfundo'"));
	if (MaterialEspacioAsset.Succeeded())
	{
		DomoCielo->SetMaterial(0, MaterialEspacioAsset.Object);
	}

	DomoCielo->SetRelativeScale3D(FVector(5000.0f, 5000.0f, 5000.0f));
	DomoCielo->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	if (Suelo)
	{
		Suelo->SetVisibility(false);
	}

	// la fabricacion de obstaculos
	FabricaObstaculos = CreateDefaultSubobject<AObstaculoEspacioFactory>(TEXT("FabricaEspacio"));
}

void AEscenarioEspacio::BeginPlay()
{
	Super::BeginPlay();

	GenerarEntornoEspacial();
}

void AEscenarioEspacio::GenerarEntornoEspacial()
{
	if (!FabricaObstaculos || !GetWorld()) return;

	int32 CantidadMeteoritos = 1000;

	for (int32 i = 0; i < CantidadMeteoritos; i++)
	{
		// Calculos para posiciones aleatorias dentro del rango del escenario
		float RndX = FMath::FRandRange(-AnchoX / 2.0f, AnchoX / 2.0f);
		float RndY = FMath::FRandRange(-LargoY / 2.0f, LargoY / 2.0f);
		float RndZ = FMath::FRandRange(-AltoZ / 2.0f, AltoZ / 2.0f);

		FVector PosicionSpawn(RndX, RndY, RndZ);
		FRotator RotacionInicial(0.0f, 0.0f, 0.0f);

		FabricaObstaculos->CrearObstaculo(GetWorld(), PosicionSpawn, RotacionInicial);
	}
}
