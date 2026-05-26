// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoMontana.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AObstaculoMontana::AObstaculoMontana()
{
	PrimaryActorTick.bCanEverTick = false;

	MallaMontana = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaMontana"));
	RootComponent = MallaMontana;

	//malla de la montana
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConoMesh(TEXT("StaticMesh'/Game/Modelos/montana/Mountain_Lowpoly.Mountain_Lowpoly'"));
	if (ConoMesh.Succeeded())
	{
		MallaMontana->SetStaticMesh(ConoMesh.Object);
	}
    //para que se activen las colisiones
	MallaMontana->SetCollisionProfileName(TEXT("BlockAllSubmissions"));
}

