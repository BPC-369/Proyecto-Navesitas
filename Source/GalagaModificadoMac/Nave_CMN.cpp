// Fill out your copyright notice in the Description page of Project Settings.


#include "Nave_CMN.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GalagaModificadoMacProjectile.h"
#include "ComponenteCombate.h"

ANave_CMN::ANave_CMN()
{
	DanioAtaque = 10.0f;        // Ataque bajo 
	Velocidad = 300.0f;        // Velocidad media 
	FrecuenciaAtaque = 0.5f;   // Intervalo de ataque medio (dispara cada 3 segundos) 


	bTieneEscudo = false;
	bEsElite = false;

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCono(TEXT("StaticMesh'/Game/Geometry/pawn/comun01.comun01'"));
	if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(FormaCono.Object);

		// Rotamos el cono para que la punta mire hacia el frente (Eje X)
		MallaEnemiga->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
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

	if (World != nullptr)
	{
		const FRotator RotacionDisparo = GetActorRotation();

		// 1. Definimos las distancias
		float DistanciaFrontal = 50.0f; // Para que no choque con la nave
		float SeparacionLateral = 80.0f; // Ajusta este número para separar más o menos las balas

		// 2. Obtenemos la posición y las direcciones de la nave
		FVector PosicionBase = GetActorLocation();
		FVector DireccionFrontal = GetActorForwardVector();
		FVector DireccionDerecha = GetActorRightVector();

		// 3. Calculamos la posición exacta de cada "cañón"
		// Cañón Derecho: Posición de la nave + Mover Adelante + Mover a la Derecha
		FVector PosicionDisparoDerecha = PosicionBase + (DireccionFrontal * DistanciaFrontal) + (DireccionDerecha * SeparacionLateral);

		// Cañón Izquierdo: Posición de la nave + Mover Adelante - Mover a la Derecha (es decir, a la izquierda)
		FVector PosicionDisparoIzquierda = PosicionBase + (DireccionFrontal * DistanciaFrontal) - (DireccionDerecha * SeparacionLateral);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// 4. Hacemos aparecer (Spawn) la bala derecha
		World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparoDerecha, RotacionDisparo, SpawnParams);

		// 5. Hacemos aparecer (Spawn) la bala izquierda
		World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparoIzquierda, RotacionDisparo, SpawnParams);
	}
}
