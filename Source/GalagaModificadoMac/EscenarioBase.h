// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ObstaculoFactory.h"
#include "EscenarioBase.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AEscenarioBase : public AActor
{
	GENERATED_BODY()

public:
	AEscenarioBase();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Escenario | Componentes")
	USceneComponent* RootEscenario;

	UPROPERTY(VisibleAnywhere, Category = "Escenario | Componentes")
	UStaticMeshComponent* ParedNorte;

	UPROPERTY(VisibleAnywhere, Category = "Escenario | Componentes")
	UStaticMeshComponent* ParedSur;

	UPROPERTY(VisibleAnywhere, Category = "Escenario | Componentes")
	UStaticMeshComponent* ParedEste;

	UPROPERTY(VisibleAnywhere, Category = "Escenario | Componentes")
	UStaticMeshComponent* ParedOeste;

	UPROPERTY(VisibleAnywhere, Category = "Escenario | Componentes")
	UStaticMeshComponent* Suelo;

	UPROPERTY(VisibleAnywhere, Category = "Escenario | Componentes")
	UStaticMeshComponent* DomoCielo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Escenario | Componentes")
	AObstaculoFactory* FabricaObstaculos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Escenario | Dimensiones")
	float AnchoX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Escenario | Dimensiones")
	float LargoY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Escenario | Dimensiones")
	float AltoZ;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Escenario | Dimensiones")
	float GrosorPared;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuracion")
	int32 CantidadObstaculosA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuracion")
	int32 CantidadObstaculosB;

	// Puntero abstracto a la fábrica inyectada por el Builder
	

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void GenerarObstaculosProcedurales();

private:

	void ConfigurarMallaPared(UStaticMeshComponent* Pared);
};