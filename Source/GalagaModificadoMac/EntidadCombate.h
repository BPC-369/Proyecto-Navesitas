// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EntidadCombate.generated.h"
//#include "Blueprint/UserWidget.h"

class UWidgetComponent;
class USceneComponent;
UCLASS()
class GALAGAMODIFICADOMAC_API AEntidadCombate : public APawn
{
	GENERATED_BODY()

public:
	AEntidadCombate();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Estadisticas")
	float VidaMaxima;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Estadisticas")
	float VidaActual;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Estadisticas")
	float EscudoMaximo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Estadisticas")
	float EscudoActual;

	// Función para la futura barra azul de escudo en la UI
	UFUNCTION(BlueprintPure, Category = "Estadisticas")
	float ObtenerPorcentajeEscudo() const;

	// funcion nativa de Unreal para hacer o recibir dano 
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Morir();

	// Le añadimos BlueprintReadOnly para que los nodos visuales tengan permiso de leerlo
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Componentes")
	UWidgetComponent* WidgetBarraVida;

	// Esta variable guardará LA CLASE del diseño (WBP_BarraVida)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> WidgetClassDeVida;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FName Faccion;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float ObtenerPorcentajeVida() const;
};
