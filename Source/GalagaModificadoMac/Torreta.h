// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Torreta.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ATorreta : public AActor
{
	GENERATED_BODY()

public:
	ATorreta();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Punteros simples para que el Garbage Collector los rastree
	class UStaticMeshComponent* MallaBase;
	class UStaticMeshComponent* MallaCanion;
	class USphereComponent* RangoDeteccion;

	// Lógica de combate interna
	float CadenciaAtaque;
	float DistanciaParaAcelerar; // Umbral para disparar más rápido 

	FTimerHandle TimerDisparo;
	void Atacar();

	class APawn* JugadorObjetivo;
public:
	// Variable de vida (según el GDD: Vida moderada) [cite: 49]
	float VidaTorreta = 1500.0f;

	// Sobrescribimos la función nativa de Unreal para recibir daño
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
