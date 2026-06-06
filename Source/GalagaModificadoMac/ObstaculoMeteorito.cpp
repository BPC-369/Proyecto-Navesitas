// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoMeteorito.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AObstaculoMeteorito::AObstaculoMeteorito()
{
	PrimaryActorTick.bCanEverTick = false;

	// malla con la referencia del asteroide
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EsferaMesh(TEXT("StaticMesh'/Game/Modelos/asteroides/Rocky_Asteroid_6.Rocky_Asteroid_6'"));
	if (EsferaMesh.Succeeded())
	{
		MallaComponent->SetStaticMesh(EsferaMesh.Object);
	}

	// material para el asteroide wiwiwiw
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatRoca(TEXT("Texture2D'/Game/StarterContent/Textures/T_Rock_Basalt_D.T_Rock_Basalt_D'"));
	if (MatRoca.Succeeded())
	{
		MallaComponent->SetMaterial(0, MatRoca.Object);
	}

	//colision
	MallaComponent->SetCollisionProfileName(TEXT("BlockAllSubmissions"));
}
