#include "NaveEnemigoAereo.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "ComponenteCombate.h" // Incluimos tu chip de vida

ANaveEnemigoAereo::ANaveEnemigoAereo()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. Creamos la malla visual
	MallaEnemiga = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEnemiga"));

	// 2. SOLUCIÓN DE RAÍZ: Como el Pawn nace sin cuerpo, convertimos la malla en el cuerpo principal
	RootComponent = MallaEnemiga;

	MallaEnemiga->SetNotifyRigidBodyCollision(true);
	MallaEnemiga->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	// 3. Instalamos el chip de estadísticas de combate
	ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 100.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	Velocidad = 500.0f;
	DanioAtaque = 1000.0f;
	FrecuenciaAtaque = 2.0f;
	bTieneEscudo = false;
}

void ANaveEnemigoAereo::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Volar(DeltaSeconds);
}

void ANaveEnemigoAereo::Volar(float DeltaSeconds)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		FVector DireccionHaciaJugador = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FVector NuevoDesplazamiento = DireccionHaciaJugador * Velocidad * DeltaSeconds;

		AddActorWorldOffset(NuevoDesplazamiento, true);

		FRotator NuevaRotacion = DireccionHaciaJugador.Rotation();
		SetActorRotation(NuevaRotacion);
	}
}

void ANaveEnemigoAereo::Atacar()
{
	
}

// 4. El puente que conecta los golpes recibidos con las matemáticas de tu chip
float ANaveEnemigoAereo::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Le pasamos el golpe al chip de matemáticas
	if (ComponenteCombate != nullptr)
	{
		DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);
	}

	return DanioReal;
}