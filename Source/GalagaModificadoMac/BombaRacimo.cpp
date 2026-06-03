#include "BombaRacimo.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"

ABombaRacimo::ABombaRacimo()
{
	// Si queremos, podemos hacer que la bomba principal viaje más lento que una bala normal
	if (GetProjectileMovement() != nullptr)
	{
		GetProjectileMovement()->InitialSpeed = 1500.f; // Más lenta
		GetProjectileMovement()->MaxSpeed = 1500.f;
	}
}

void ABombaRacimo::BeginPlay()
{
	Super::BeginPlay();

	// Iniciamos el temporizador: si no choca con nada en 1.5 segundos, explota sola
	GetWorld()->GetTimerManager().SetTimer(TimerExplosion, this, &ABombaRacimo::Explotar, 1.5f, false);
}

void ABombaRacimo::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Verificamos que no estemos chocando con la nave que nos disparó
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherActor != GetOwner()))
	{
		// En lugar de hacer daño normal (como el padre), la bomba simplemente explota
		Explotar();
	}
}

void ABombaRacimo::Explotar()
{
	// Cancelamos el temporizador por si la explosión fue causada por un choque
	GetWorld()->GetTimerManager().ClearTimer(TimerExplosion);

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		// Hacemos nacer 12 balas normales (del PADRE) en todas direcciones
		for (int i = 0; i < 12; i++)
		{
			FRotator RotacionMiniBala = GetActorRotation();
			RotacionMiniBala.Yaw += (30.0f * i);

			// Fíjate que aquí llamamos a AGalagaModificadoMacProjectile (la bala normal), no a la bomba.
			World->SpawnActor<AGalagaModificadoMacProjectile>(GetActorLocation(), RotacionMiniBala, SpawnParams);
		}
	}

	// Destruimos la carcasa de la bomba principal
	Destroy();
}