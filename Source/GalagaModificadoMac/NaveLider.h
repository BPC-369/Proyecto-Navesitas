// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "NaveLider.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ANaveLider : public ANaveEnemigoAereo
{
	GENERATED_BODY()

public:
	ANaveLider();

	// Atributos específicos del Líder 
	int32 ContadorDisparos;
	float EnergiaEscudo;
	float VidaLider; 

	// Sobrescribimos el ataque y el daño [cite: 45, 62]
	virtual void Atacar() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

public:
	// Aquí está el Tick que mencionas
	virtual void Tick(float DeltaTime) override;

private:
	FTimerHandle TimerAtaque;
};
