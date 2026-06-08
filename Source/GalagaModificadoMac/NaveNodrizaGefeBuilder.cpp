// Fill out your copyright notice in the Description page of Project Settings.


#include "NaveNodrizaGefeBuilder.h"
#include "EscenarioNaveNodriza.h"
#include "Components/StaticMeshComponent.h"

void ANaveNodrizaGefeBuilder::ConstruirEsteticaCielo()
{
	if (!EscenarioEnConstruccion) return;

	UMaterialInterface* MaterialJefeAsset = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/Modelos/gefefinal/MFinal.MFinal'"));

	if (MaterialJefeAsset && EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetMaterial(0, MaterialJefeAsset);
	}

	if (EscenarioEnConstruccion->DomoCielo)
	{
		EscenarioEnConstruccion->DomoCielo->SetRelativeScale3D(FVector(2500.0f)); // Más grande para el Boss
	}
}

