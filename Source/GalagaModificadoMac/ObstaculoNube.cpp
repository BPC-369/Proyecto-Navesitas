// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoNube.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AObstaculoNube::AObstaculoNube()
{
	PrimaryActorTick.bCanEverTick = false;

	// malla de nubes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EsferaMesh(TEXT("StaticMesh'/Game/Modelos/nubes/Cloud.CLOUD'"));
	if (EsferaMesh.Succeeded())
	{
		MallaComponent->SetStaticMesh(EsferaMesh.Object);
	}
/*
	// material(color) de nubes
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatDefecto(TEXT("Material'/Game/Modelos/Atmosfera/Mcieloapocaliptico.Mcieloapocaliptico'"));
	if (MatDefecto.Succeeded())
	{
		MallaComponent->SetMaterial(0, MatDefecto.Object);
	}
	*/

	//para que tenga colision
	MallaComponent->SetCollisionProfileName(TEXT("BlockAllSubmissions"));
}

