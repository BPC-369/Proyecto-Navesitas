#pragma once

#include "CoreMinimal.h"
#include "NaveEnemigoAereo.h"

// Definici?n de la clase FabricaNaves
class GALAGAMODIFICADOMAC_API FabricaNaves
{
public:
	// Enumerador para identificar el tipo de nave que queremos crear por ahora solo COMUN.
	enum TipoNave { COMUN, KAMIKASE };

	// M?todo est?tico que devuelve un puntero a la nave creada. Al ser est?tico, no necesitamos instanciar la f?brica.
	static ANaveEnemigoAereo* CrearNave(TipoNave Tipo, UWorld* World, FVector Ubicacion, FRotator Rotacion);
};