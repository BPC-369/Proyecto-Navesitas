#include "FabricaNaves.h"
#include "Nave_CMN.h"
#include "NaveKamikase.h"

// Implementaci?n del m?todo para crear naves
ANaveEnemigoAereo* FabricaNaves::CrearNave(TipoNave Tipo, UWorld* World, FVector Ubicacion, FRotator Rotacion)
{
	// Si el mundo no existe por ejemplo el juego se est? cerrando retornamos nulo por seguridad
	if (!World) return nullptr;

	// Estructura que define c?mo debe nacer el actor en el mundo
	FActorSpawnParameters Params;
	// Obligamos a que la nave aparezca siempre, incluso si colisiona con algo al nacer
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Evaluamos qu? tipo de nave nos pidieron crear
	switch (Tipo)
	{
	case COMUN:
		// Instanciamos la Nave Com?n en el Mundo y devolvemos su puntero
		return World->SpawnActor<ANave_CMN>(ANave_CMN::StaticClass(), Ubicacion, Rotacion, Params);
	case KAMIKASE:
		return World->SpawnActor<ANaveKamikase>(ANaveKamikase::StaticClass(), Ubicacion, Rotacion, Params);

	default:
		// Si mandan un tipo no reconocido, no devolvemos nada
		return nullptr;
	}
}