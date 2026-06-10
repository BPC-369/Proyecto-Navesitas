#include "BombaRacimo.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

ABombaRacimo::ABombaRacimo()
{
	if (GetProjectileMovement())
	{
		GetProjectileMovement()->InitialSpeed = 1500.f;
		GetProjectileMovement()->MaxSpeed = 1500.f;
	}
}

void ABombaRacimo::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerExplosion, this, &ABombaRacimo::Explotar, 1.5f, false);
}

void ABombaRacimo::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	Explotar();
}

void ABombaRacimo::Explotar()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerExplosion);

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		for (int i = 0; i < 12; i++)
		{
			FRotator RotacionMiniBala = GetActorRotation();
			RotacionMiniBala.Yaw += (30.0f * i);

			// El proyectil se configura con su velocidad automáticamente, no necesita Init
			World->SpawnActor<AGalagaModificadoMacProjectile>(
				GetActorLocation(), RotacionMiniBala, SpawnParams);
		}
	}

	Destroy();
}