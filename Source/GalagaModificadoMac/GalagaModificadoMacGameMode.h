// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GalagaModificadoMacGameMode.generated.h"

UCLASS(MinimalAPI)
class AGalagaModificadoMacGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGalagaModificadoMacGameMode();
protected:
	// Sobrescribimos el BeginPlay para que ocurra al darle "Play"
	virtual void BeginPlay() override;
};



