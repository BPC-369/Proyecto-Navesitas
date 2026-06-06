// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoMontana.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AObstaculoMontana::AObstaculoMontana()
{
	PrimaryActorTick.bCanEverTick = false;

	//malla de la montana
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConoMesh(TEXT("StaticMesh'/Game/Modelos/montana/Mountain_Lowpoly.Mountain_Lowpoly'"));
	if (ConoMesh.Succeeded())
	{
		MallaComponent->SetStaticMesh(ConoMesh.Object);
	}
    //para que se activen las colisiones
	MallaComponent->SetCollisionProfileName(TEXT("BlockAllSubmissions"));
}

