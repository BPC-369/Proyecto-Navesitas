// Fill out your copyright notice in the Description page of Project Settings.


#include "Nave_CMN.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h" 
#include "GalagaModificadoMacProjectile.h"
#include "ComponenteCombate.h"

ANave_CMN::ANave_CMN()
{
	DanioAtaque = 10.0f;        // Ataque bajo 
	Velocidad = 300.0f;        // Velocidad media 
	FrecuenciaAtaque = 0.5f;   // Intervalo de ataque medio (dispara cada 0.5 segundos) 

	bTieneEscudo = false;
	bEsElite = false;

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCono(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow06.StarSparrow06'"));
	if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(FormaCono.Object);

		// Rotamos la malla para que mire hacia el frente real (Eje X)
		MallaEnemiga->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
		MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void ANave_CMN::BeginPlay()
{
	Super::BeginPlay();

	// Iniciamos el ciclo de disparo automático en cuanto la nave nace en el nivel
	GetWorld()->GetTimerManager().SetTimer(TemporizadorAtaque, this, &ANave_CMN::Atacar, FrecuenciaAtaque, true);
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

		// 1. Definimos las distancias de los cañones saliendo del radio de la malla
		float DistanciaFrontal = 120.0f;
		float SeparacionLateral = 80.0f;

		// 2. Obtenemos la posición y las direcciones reales del Actor raíz
		FVector PosicionBase = GetActorLocation();
		FVector DireccionFrontal = GetActorForwardVector();
		FVector DireccionDerecha = GetActorRightVector();

		// 3. Calculamos la posición exacta de cada cañón afuera de la nave
		FVector PosicionDisparoDerecha = PosicionBase + (DireccionFrontal * DistanciaFrontal) + (DireccionDerecha * SeparacionLateral);
		FVector PosicionDisparoIzquierda = PosicionBase + (DireccionFrontal * DistanciaFrontal) - (DireccionDerecha * SeparacionLateral);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// 4. Spawneamos el proyectil derecho y usamos la función correcta: IgnoreActorWhenMoving
		AGalagaModificadoMacProjectile* ProyectilDerecho = World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparoDerecha, RotacionDisparo, SpawnParams);
		if (ProyectilDerecho)
		{
			UPrimitiveComponent* ColliderDerecho = ProyectilDerecho->FindComponentByClass<UPrimitiveComponent>();
			if (ColliderDerecho)
			{
				ColliderDerecho->IgnoreActorWhenMoving(this, true); // <--- CORRECCIÓN AQUÍ
			}
		}

		// 5. Spawneamos el proyectil izquierdo haciendo exactamente lo mismo
		AGalagaModificadoMacProjectile* ProyectilIzquierdo = World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparoIzquierda, RotacionDisparo, SpawnParams);
		if (ProyectilIzquierdo)
		{
			UPrimitiveComponent* ColliderIzquierdo = ProyectilIzquierdo->FindComponentByClass<UPrimitiveComponent>();
			if (ColliderIzquierdo)
			{
				ColliderIzquierdo->IgnoreActorWhenMoving(this, true); // <--- CORRECCIÓN AQUÍ
			}
		}
	}
}