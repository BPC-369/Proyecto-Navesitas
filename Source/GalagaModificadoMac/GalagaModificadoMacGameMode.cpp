// Copyright Epic Games, Inc. All Rights Reserved.

#include "GalagaModificadoMacGameMode.h"
#include "GalagaModificadoMacPawn.h"

AGalagaModificadoMacGameMode::AGalagaModificadoMacGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AGalagaModificadoMacPawn::StaticClass();
}

