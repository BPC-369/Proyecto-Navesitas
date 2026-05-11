// Fill out your copyright notice in the Description page of Project Settings.


#include "Torreta.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GalagaModificadoMacProjectile.h"
#include "UObject/ConstructorHelpers.h"

ATorreta::ATorreta()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. Creación de componentes sin exposición a Blueprints
	MallaBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaBase"));
	RootComponent = MallaBase;

	MallaCanion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCanion"));
	MallaCanion->SetupAttachment(MallaBase);

	RangoDeteccion = CreateDefaultSubobject<USphereComponent>(TEXT("RangoDeteccion"));
	RangoDeteccion->SetupAttachment(RootComponent);

	// Configuración inicial de atributos 
	RangoDeteccion->SetSphereRadius(1200.0f);
	CadenciaAtaque = 1.0f;
	DistanciaParaAcelerar = 600.0f; // Si estás a menos de esto, se vuelve loca 

	// Carga de malla por código (puedes usar un cilindro del motor)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (MeshAsset.Succeeded())
	{
		MallaBase->SetStaticMesh(MeshAsset.Object);
		MallaCanion->SetStaticMesh(MeshAsset.Object);
		MallaCanion->SetRelativeScale3D(FVector(0.5f, 0.5f, 2.0f)); // Cañón largo
	}
}

void ATorreta::BeginPlay()
{
	Super::BeginPlay();
	JugadorObjetivo = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// Iniciamos el ciclo de disparo básico
	GetWorldTimerManager().SetTimer(TimerDisparo, this, &ATorreta::Atacar, CadenciaAtaque, true);
}

void ATorreta::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (JugadorObjetivo && RangoDeteccion->IsOverlappingActor(JugadorObjetivo))
	{
		// Rotación suave hacia el jugador
		FVector Direccion = JugadorObjetivo->GetActorLocation() - MallaCanion->GetComponentLocation();
		FRotator NuevaRotacion = Direccion.Rotation();
		MallaCanion->SetWorldRotation(FMath::RInterpTo(MallaCanion->GetComponentRotation(), NuevaRotacion, DeltaTime, 5.0f));

		float DistanciaActual = FVector::Dist(GetActorLocation(), JugadorObjetivo->GetActorLocation());

		// Solo cambiamos el timer si la cadencia actual es diferente a la necesaria
		float NuevaCadencia = (DistanciaActual < DistanciaParaAcelerar) ? 0.5f : CadenciaAtaque;

		// Comprobamos si el timer ya tiene la cadencia correcta para no resetearlo cada frame
		if (!GetWorldTimerManager().IsTimerActive(TimerDisparo) || GetWorldTimerManager().GetTimerRate(TimerDisparo) != NuevaCadencia)
		{
			GetWorldTimerManager().SetTimer(TimerDisparo, this, &ATorreta::Atacar, NuevaCadencia, true);
		}
	}
}

void ATorreta::Atacar()
{
	// Solo disparamos si el jugador está vivo y en el rango [cite: 49]
	if (JugadorObjetivo && RangoDeteccion->IsOverlappingActor(JugadorObjetivo))
	{
		// 1. Calculamos el punto de salida (fuera de la colisión de la torreta)
		FVector SpawnLoc = MallaCanion->GetComponentLocation() + (MallaCanion->GetForwardVector() * 200.0f);
		FRotator SpawnRot = MallaCanion->GetComponentRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		// Forzamos el spawn aunque haya algo cerca
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (GetWorld())
		{
			// Spawneamos usando la clase exacta de tu proyecto
			AActor* Proyectil = GetWorld()->SpawnActor<AGalagaModificadoMacProjectile>(AGalagaModificadoMacProjectile::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);

			if (Proyectil)
			{
				// CONTROL VISUAL: Hacemos la bala 5 veces más grande para verla
				Proyectil->SetActorScale3D(FVector(5.0f, 5.0f, 5.0f));

				// CONTROL DE VELOCIDAD: Si quieres que vaya más lento para ver el trayecto
				// Castamos al proyectil para acceder a su movimiento si es necesario
				UE_LOG(LogTemp, Warning, TEXT("!!! TORRETA DISPARÓ UNA BALA GIGANTE !!!"));
			}
		}
	}
}
float ATorreta::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	VidaTorreta -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("Torreta impactada! Vida restante: %f"), VidaTorreta);

	if (VidaTorreta <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Torreta Destruida!"));

		// Limpiamos el Timer para que no intente disparar después de muerta
		GetWorldTimerManager().ClearTimer(TimerDisparo);

		Destroy(); // Se elimina del mundo
	}

	return DamageAmount;
}