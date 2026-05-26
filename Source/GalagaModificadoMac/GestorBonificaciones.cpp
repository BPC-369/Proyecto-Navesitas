#include "GestorBonificaciones.h"
#include "Bonificacion.h"

AGestorBonificaciones::AGestorBonificaciones()
{
	PrimaryActorTick.bCanEverTick = false;

	// Ajusta esto según el tamaño de tus escenarios
	LimiteX = 3000.0f;
	LimiteY = 3000.0f;
}

void AGestorBonificaciones::BeginPlay()
{
	Super::BeginPlay();

	// Intenta generar una bonificación cada 4 segundos
	GetWorldTimerManager().SetTimer(TimerGeneracion, this, &AGestorBonificaciones::IntentarGenerarBonificacion, 4.0f, true);
}

void AGestorBonificaciones::IntentarGenerarBonificacion()
{
	// Leemos la variable estática de la clase Bonificacion. ¡Puro C++!
	if (ABonificacion::CantidadActivas < 3)
	{
		// Generamos coordenadas aleatorias
		float RandX = FMath::RandRange(-LimiteX, LimiteX);
		float RandY = FMath::RandRange(-LimiteY, LimiteY);

		// OJO: Ajusta la Z para que aparezcan a la altura correcta de la nave o del robot
		FVector PosicionSpawn = FVector(GetActorLocation().X + RandX, GetActorLocation().Y + RandY, GetActorLocation().Z);

		GetWorld()->SpawnActor<ABonificacion>(PosicionSpawn, FRotator::ZeroRotator);
	}
}