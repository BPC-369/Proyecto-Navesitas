// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemigoTerrestre.h"
#include "Robot_Lider.generated.h"

/**
 * 
 */
UCLASS()
class GALAGAMODIFICADOMAC_API ARobot_Lider : public AEnemigoTerrestre
{
	GENERATED_BODY()
public:
	ARobot_Lider();

protected:
	virtual void Tick(float DeltaTime);
	virtual void BeginPlay() override;

	// --- Atributos del Documento ---

	// Capacidad de invocar unidades comunes (Robot_RZ o Nave_CMN)
	UFUNCTION(BlueprintCallable, Category = "Habilidades")
	void InvocarRefuerzos();

	// Ataque devastador en área
	UFUNCTION(BlueprintCallable, Category = "Habilidades")
	void EjecutarAtaqueArea();

	// Tipos de clases que puede invocar (Configurable desde el editor)
	UPROPERTY(EditAnywhere, Category = "Comandante")
	TSubclassOf<AActor> ClaseParaInvocar;

	UPROPERTY(EditAnywhere, Category = "Estadisticas")
	float RadioAtaqueArea;

	UPROPERTY(EditAnywhere, Category = "Estadisticas")
	float DanioArea;

private:
	// Temporizadores para automatizar el comportamiento
	FTimerHandle TimerHandle_Invocacion;
	FTimerHandle TimerHandle_AtaqueArea;
};
