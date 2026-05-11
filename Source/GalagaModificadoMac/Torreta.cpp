#include "Torreta.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GalagaModificadoMacProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "ComponenteCombate.h"
#include "GameFramework/CharacterMovementComponent.h" // Necesario para inmovilizarla

ATorreta::ATorreta()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. CUMPLIENDO EL DISEÑO: Como hereda de Character, la inmovilizamos para que sea una torreta estática.
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	}

	// 2. ARREGLO VITAL: ¡Construir los componentes ANTES de usarlos!
	MallaCanion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCanion"));
	MallaCanion->SetupAttachment(MallaEnemiga); // Enganchamos el cañón a la base

	RangoDeteccion = CreateDefaultSubobject<USphereComponent>(TEXT("RangoDeteccion"));
	RangoDeteccion->SetupAttachment(RootComponent);

	// Ahora sí podemos cambiar el radio sin crashear
	RangoDeteccion->SetSphereRadius(1200.0f);
	CadenciaAtaque = 1.0f;
	DistanciaParaAcelerar = 600.0f;

	// 3. Ajuste de Vida (Vida moderada según tu diseño)
	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 150.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	// Cargamos el cilindro nativo de Unreal como base
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaBase(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (FormaBase.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(FormaBase.Object);
	}
}

void ATorreta::BeginPlay()
{
	Super::BeginPlay();
	JugadorObjetivo = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	GetWorldTimerManager().SetTimer(TimerDisparo, this, &ATorreta::Atacar, CadenciaAtaque, true);
}

void ATorreta::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (JugadorObjetivo && RangoDeteccion->IsOverlappingActor(JugadorObjetivo))
	{
		// Rotación suave exclusiva del CAÑÓN hacia el jugador
		FVector Direccion = JugadorObjetivo->GetActorLocation() - MallaCanion->GetComponentLocation();
		FRotator NuevaRotacion = Direccion.Rotation();

		// TRUCO OPCIONAL: Si no quieres que el cañón mire hacia arriba/abajo, descomenta estas líneas:
		// NuevaRotacion.Pitch = 0.0f; 
		// NuevaRotacion.Roll = 0.0f;

		MallaCanion->SetWorldRotation(FMath::RInterpTo(MallaCanion->GetComponentRotation(), NuevaRotacion, DeltaTime, 5.0f));

		float DistanciaActual = FVector::Dist(GetActorLocation(), JugadorObjetivo->GetActorLocation());
		float NuevaCadencia = (DistanciaActual < DistanciaParaAcelerar) ? 0.5f : CadenciaAtaque;

		if (!GetWorldTimerManager().IsTimerActive(TimerDisparo) || GetWorldTimerManager().GetTimerRate(TimerDisparo) != NuevaCadencia)
		{
			GetWorldTimerManager().SetTimer(TimerDisparo, this, &ATorreta::Atacar, NuevaCadencia, true);
		}
	}
}

void ATorreta::Atacar()
{
	if (JugadorObjetivo && RangoDeteccion->IsOverlappingActor(JugadorObjetivo))
	{
		// 1. AUMENTAMOS EL OFFSET: Como la bala es gigante (5x), 200.0f no alcanza. 
		// Lo subimos a 400.0f o 500.0f para que nazca bien afuera de la torreta.
		FVector SpawnLoc = MallaCanion->GetComponentLocation() + (MallaCanion->GetForwardVector() * 500.0f);
		FRotator SpawnRot = MallaCanion->GetComponentRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this; // ¡VITAL! Declaramos a la Torreta como "dueña" de la bala
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (GetWorld())
		{
			AActor* Proyectil = GetWorld()->SpawnActor<AGalagaModificadoMacProjectile>(AGalagaModificadoMacProjectile::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);

			if (Proyectil)
			{
				Proyectil->SetActorScale3D(FVector(5.0f, 5.0f, 5.0f));

				// 2. EL SEGURO DE VIDA: Forzamos a la raíz de la bala a ignorar a la torreta físicamente
				UPrimitiveComponent* ColisionBala = Cast<UPrimitiveComponent>(Proyectil->GetRootComponent());
				if (ColisionBala)
				{
					// La bala atravesará a la torreta en caso de que choquen accidentalmente
					ColisionBala->IgnoreActorWhenMoving(this, true);
				}
			}
		}
	}
}