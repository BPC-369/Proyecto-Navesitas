// Fill out your copyright notice in the Description page of Project Settings.


#include "Nave_CMN.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GalagaModificadoMacProjectile.h"

ANave_CMN::ANave_CMN()
{
	VidaMaxima = 50.0f;        // Vida baja 
	VidaActual = VidaMaxima;
	DanioAtaque = 10.0f;        // Ataque bajo 
	Velocidad = 300.0f;        // Velocidad media 
	FrecuenciaAtaque = 0.5f;   // Intervalo de ataque medio (dispara cada 3 segundos) 

	bTieneEscudo = false;
	bEsElite = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCono(TEXT("StaticMesh'/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO'"));
	if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(FormaCono.Object);

		// Rotamos el cono para que la punta mire hacia el frente (Eje X)
		MallaEnemiga->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
}

void ANave_CMN::BeginPlay()
{
	Super::BeginPlay();

	// Iniciamos el ciclo de disparo automático en cuanto la nave nace en el nivel
	GetWorld()->GetTimerManager().SetTimer(TemporizadorAtaque, this, &ANave_CMN::Atacar, FrecuenciaAtaque, true);
	//ConvertirAElite();
}

void ANave_CMN::ConvertirAElite()
{
	if (!bEsElite)
	{
		bEsElite = true;

		Velocidad = 900.0f;           // Más rápido 
		FrecuenciaAtaque = 0.25f;      // Frecuencia de disparo más agresiva 

		// Reiniciamos el temporizador con la nueva frecuencia agresiva
		GetWorld()->GetTimerManager().ClearTimer(TemporizadorAtaque);
		GetWorld()->GetTimerManager().SetTimer(TemporizadorAtaque, this, &ANave_CMN::Atacar, FrecuenciaAtaque, true);
	}
}

void ANave_CMN::Atacar()
{
	UWorld* const World = GetWorld();

	// Escudo protector: Validamos que el mundo exista
	if (World != nullptr)
	{
		//Tomamos la rotación actual de la nave para que la bala salga disparada hacia adelante.
		const FRotator RotacionDisparo = GetActorRotation();

		//Colocamos la Bala un poco por delante de la nave para que no choque con ella al nacer.
		const FVector PosicionDisparo = GetActorLocation() + (GetActorForwardVector() * 100.0f);

		// indicamos que esta nave es la dueña de la bala que va a nacer
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// Le pasamos SpawnParams al final para que la bala nazca sabiendo quién es su padre.
		//World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparo, RotacionDisparo, SpawnParams);
	}
}
