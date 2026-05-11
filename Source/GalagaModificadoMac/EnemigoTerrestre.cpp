#include "EnemigoTerrestre.h"
#include "Kismet/GameplayStatics.h" 
#include "ComponenteCombate.h" // Incluimos tu chip
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h" // Para la cápsula raíz del ACharacter
#include "GameFramework/CharacterMovementComponent.h" // Para la gravedad y físicas

AEnemigoTerrestre::AEnemigoTerrestre()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. Configuramos el motor de movimiento del ACharacter
	GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;

	// 2. Pegamos la malla a la cápsula que ya viene por defecto en los ACharacter
	MallaEnemiga = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEnemiga"));
	MallaEnemiga->SetupAttachment(GetCapsuleComponent());
	MallaEnemiga->SetNotifyRigidBodyCollision(true);

	// 3. ¡Instalamos tu chip de combate!
	ComponenteCombate = CreateDefaultSubobject<UComponenteCombate>(TEXT("EstadisticasCombate"));
	
	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}
	VelocidadMovimiento = 300.0f;
	RangoDeteccion = 500.0f;
	DanioBase = 10.0f;
	RangoAtaque = 150.0f;
}

void AEnemigoTerrestre::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ActualizarComportamiento();
}

void AEnemigoTerrestre::Mover()
{
	APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Jugador)
	{
		FVector MiPosicion = GetActorLocation();
		FVector PosicionJugador = Jugador->GetActorLocation();

		FVector DireccionHaciaJugador = PosicionJugador - MiPosicion;
		DireccionHaciaJugador.Z = 0.0f;
		DireccionHaciaJugador.Normalize();

		// 4. EL CAMBIO MAESTRO: En lugar de empujar el actor a la fuerza, usamos el sistema nativo.
		// Esto activa las animaciones de caminar, respeta la gravedad y las colisiones con la ciudad.
		AddMovementInput(DireccionHaciaJugador, 1.0f);
	}
}

float AEnemigoTerrestre::CalcularDistanciaAlJugador()
{
	APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Jugador)
	{
		return FVector::Distance(GetActorLocation(), Jugador->GetActorLocation());
	}
	return -1.0f;
}

bool AEnemigoTerrestre::DetectarObjetivo()
{
	float Distancia = CalcularDistanciaAlJugador();
	return (Distancia > 0.0f && Distancia <= RangoDeteccion);
}

void AEnemigoTerrestre::ActualizarComportamiento()
{
	bool bJugadorEnLaMira = DetectarObjetivo();

	if (bJugadorEnLaMira)
	{
		Mover();

		float DistanciaActual = CalcularDistanciaAlJugador();
		if (DistanciaActual <= RangoAtaque)
		{
			Atacar();
		}
	}
}

void AEnemigoTerrestre::Atacar()
{

}

// 5. El puente de daño hacia tu componente
float AEnemigoTerrestre::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Le pasamos el golpe al chip de matemáticas
	if (ComponenteCombate != nullptr)
	{
		DanioReal = ComponenteCombate->HacerDamage(DanioReal, DamageEvent, EventInstigator, DamageCauser);
	}

	return DanioReal;
}