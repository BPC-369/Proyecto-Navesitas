// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstaculoAtmosferaFactory.h"
#include "ObstaculoNube.h"
#include "ObstaculoMontana.h"
#include "Engine/World.h"

AObstaculoAtmosferaFactory::AObstaculoAtmosferaFactory() {}

AObstaculoDestruido* AObstaculoAtmosferaFactory::CrearObstaculo(UWorld* Mundo, FVector Posicion, FRotator Rotacion)
{
	return nullptr;
}
//en esta parte configuramos la creacion de nubesitas 
AObstaculoDestruido* AObstaculoAtmosferaFactory::CrearNubeEspecifica(UWorld* Mundo, FVector Posicion, float EscalaX, float EscalaY, float EscalaZ)
{
	if (!Mundo) return nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AObstaculoNube* NuevaNube = Mundo->SpawnActor<AObstaculoNube>(AObstaculoNube::StaticClass(), Posicion, FRotator::ZeroRotator, SpawnParams);
	if (NuevaNube)
	{
		NuevaNube->SetActorScale3D(FVector(EscalaX, EscalaY, EscalaZ));
		NuevaNube->SetActorRotation(FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f));
	}
	return NuevaNube;
}
//aca de las montanitas
AObstaculoDestruido* AObstaculoAtmosferaFactory::CrearMontanaEspecifica(UWorld* Mundo, FVector Posicion)
{
	if (!Mundo) return nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AObstaculoMontana* NuevaMontana = Mundo->SpawnActor<AObstaculoMontana>(AObstaculoMontana::StaticClass(), Posicion, FRotator::ZeroRotator, SpawnParams);
	if (NuevaMontana)
	{
		float Ancho = FMath::FRandRange(15.0f, 25.0f);
		float Alto = FMath::FRandRange(40.0f, 70.0f);
		NuevaMontana->SetActorScale3D(FVector(Ancho, Ancho, Alto));
		NuevaMontana->SetActorRotation(FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f));
	}
	return NuevaMontana;
}
