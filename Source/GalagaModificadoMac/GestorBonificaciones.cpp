#include "GestorBonificaciones.h"
#include "Bonificacion.h"
#include "EscenarioBase.h"
#include "Kismet/GameplayStatics.h"

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
	if (ABonificacion::CantidadActivas < 3)
	{
		float RndX = 0.0f;
		float RndY = 0.0f;
		float AlturaZ = 1290.0f; // Altura estándar de juego

		AActor* EscenarioActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEscenarioBase::StaticClass());
		AEscenarioBase* EscenarioActual = Cast<AEscenarioBase>(EscenarioActor);

		if (EscenarioActual)
		{
			float RadioX = (EscenarioActual->AnchoX / 2.0f) - 1000.0f;
			float RadioY = (EscenarioActual->LargoY / 2.0f) - 1000.0f;

			RndX = FMath::FRandRange(-RadioX, RadioX);
			RndY = FMath::FRandRange(-RadioY, RadioY);

			AlturaZ = (EscenarioActual->AnchoX > 50000.0f) ? 100.0f : 1290.0f;
		}
		else
		{
			RndX = FMath::FRandRange(-LimiteX, LimiteX);
			RndY = FMath::FRandRange(-LimiteY, LimiteY);
		}

		FVector PosicionSpawn = FVector(RndX, RndY, AlturaZ);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<ABonificacion>(ABonificacion::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, SpawnParams);
	}
}