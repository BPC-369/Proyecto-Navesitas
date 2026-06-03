// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoNube.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AObstaculoNube::AObstaculoNube()
{
	PrimaryActorTick.bCanEverTick = false;

	MallaNube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaNube"));
	RootComponent = MallaNube;

	// malla de nubes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EsferaMesh(TEXT("StaticMesh'/Game/Modelos/nubes/Cloud.CLOUD'"));
	if (EsferaMesh.Succeeded())
	{
		MallaNube->SetStaticMesh(EsferaMesh.Object);
	}

	// material(color) de nubes
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatDefecto(TEXT("Material'/Game/Modelos/nubes/Material.Material'"));
	if (MatDefecto.Succeeded())
	{
		MallaNube->SetMaterial(0, MatDefecto.Object);
	}

	//para que tenga colision
	MallaNube->SetCollisionProfileName(TEXT("BlockAllSubmissions"));
}

