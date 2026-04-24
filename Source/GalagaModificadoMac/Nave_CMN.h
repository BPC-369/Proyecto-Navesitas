// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"
#include "Nave_CMN.generated.h"

/**
 * 
 */
UCLASS()
class GALAGAMODIFICADOMAC_API ANave_CMN : public ANaveEnemigoAereo
{
	GENERATED_BODY()
public:
	ANave_CMN();

	// Transforma la nave en su versión Élite en tiempo de ejecución
	void ConvertirAElite();

protected:
	virtual void BeginPlay() override;

	// Sobrescribimos el ataque base para darle el patrón simple de la Nave Común
	virtual void Atacar() override;

private:
	// Identificador para saber si esta nave es élite
	bool bEsElite;

	// Temporizador puro de C++ para gestionar el intervalo de ataque medio
	FTimerHandle TemporizadorAtaque;

};
