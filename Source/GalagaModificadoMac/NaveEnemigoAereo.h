// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntidadCombate.h"
#include "NaveEnemigoAereo.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class GALAGAMODIFICADOMAC_API ANaveEnemigoAereo : public AEntidadCombate
{
	GENERATED_BODY()

public:
    ANaveEnemigoAereo();

    // Atributos comunes definidos en el concepto del juego
    float Velocidad;           // 
    float DanioAtaque;         // 
    float FrecuenciaAtaque;    // [cite: 42, 44]
    bool bTieneEscudo;         // Para variantes Élite o Líder [cite: 43, 45]

    // Métodos que heredarán todas las naves hijas
    virtual void Tick(float DeltaSeconds) override;
	//virtual void BeginPlay() override;

    // Función de vuelo tridimensional (Polimorfismo)
    virtual void Volar(float DeltaSeconds);

    // Función de ataque (será diferente para cada tipo de nave)
    virtual void Atacar();
    //virtual void AlChocar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
    // Componente visual para la nave enemiga
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
    class UStaticMeshComponent* MallaEnemiga;
	
};
