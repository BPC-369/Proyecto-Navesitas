// Fill out your copyright notice in the Description page of Project Settings.


#include "Robot_Razo.h"

ARobot_Razo::ARobot_Razo()
{
	PrimaryActorTick.bCanEverTick = true;

	VelocidadMovimiento = 250.0f;
	RangoDeteccion = 800.0f;
	DanioBase = 15.0f;

	VidaMaxima = 100.0f;
	VidaActual = VidaMaxima;
	bProtocoloFuriaActivado = false;
}

void ARobot_Razo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Lógica Única de la familia Razo: Si la vida cae al 50% o menos
	if (!bProtocoloFuriaActivado && VidaActual <= (VidaMaxima * 0.5f))
	{
		// Regeneración y aumento de estadísticas
		VidaActual += 20.0f; // Se cura
		DanioBase *= 1.5f;   // Sube su ataque un 50%
		bProtocoloFuriaActivado = true; // Bloqueamos para que no se cure infinitamente
	}
}