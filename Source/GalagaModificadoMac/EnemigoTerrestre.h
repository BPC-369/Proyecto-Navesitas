// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntidadCombate.h"
#include "EnemigoTerrestre.generated.h"

class UStaticMeshComponent;
UCLASS(Abstract)
class GALAGAMODIFICADOMAC_API AEnemigoTerrestre : public AEntidadCombate
{
	GENERATED_BODY()
protected:
    float VelocidadMovimiento;
    float RangoDeteccion;
    float DanioBase;
    float RangoAtaque;
    UStaticMeshComponent* MallaEnemiga;
public:
    AEnemigoTerrestre();

	virtual void Tick(float DeltaTime) override;
    virtual void Mover();
    virtual void ActualizarComportamiento();
    virtual bool DetectarObjetivo();
    virtual float CalcularDistanciaAlJugador();
    virtual void Atacar();
};
