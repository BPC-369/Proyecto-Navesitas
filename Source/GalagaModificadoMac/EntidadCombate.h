// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EntidadCombate.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEntidadCombate : public APawn
{
	GENERATED_BODY()

public:
	AEntidadCombate();

protected:
	virtual void BeginPlay() override;

	float VidaMaxima;
	float VidaActual;

	// funcion nativa de Unreal para hacer o recibir dano 
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Morir();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FName Faccion;
};
