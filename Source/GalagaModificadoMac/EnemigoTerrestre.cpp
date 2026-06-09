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
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

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

	// 2. EL CUELLO: Desactivamos la rotación fija y dejamos que el motor de movimiento lo gire
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f); // Qué tan rápido gira (600 es buen número)
	GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	GetCharacterMovement()->GravityScale = 2.0f; // Gravedad extra para que caigan rápido al nacer
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

		//SetActorRotation(DireccionHaciaJugador.Rotation());
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
		// 1. EL MOVIMIENTO SIEMPRE VA PRIMERO
		// Esto garantiza que el Francotirador haga su Kiting/Zigzag 
		// y que el Espadachín camine mientras da el golpe.
		Mover();

		// 2. EL CANDADO DE ANIMACIÓN
		// Si el espadachín está dando un tajo, pausamos su cerebro aquí 
		// para que el Abuelo no le corte la animación cambiando la velocidad.
		if (bEstaAtacando) return;

		float DistanciaActual = CalcularDistanciaAlJugador();
		if (DistanciaActual <= RangoAtaque)
		{
			// 3. MODO ATAQUE
			// ¡EL CANDADO DEL MÉDICO! Solo cambiamos la velocidad si NO está en tratamiento
			if (!bEstaSiendoCurado)
			{
				GetCharacterMovement()->MaxWalkSpeed = 100.0f;
			}

			Atacar();
		}
		else
		{
			// 4. MODO PERSECUCIÓN
			if (!bEstaSiendoCurado)
			{
				GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			}
		}
	}
	else
	{
		// 5. MODO MERODEO
		// Por si acaso dejamos de ver al jugador justo al dar un golpe
		if (bEstaAtacando) return;

		if (!bEstaSiendoCurado)
		{
			GetCharacterMovement()->MaxWalkSpeed = 150.0f;
		}

		Merodear();
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

void AEnemigoTerrestre::Merodear()
{
	// Descontamos tiempo
	TiempoParaCambiarRuta -= GetWorld()->GetDeltaSeconds();

	// Si el tiempo se acaba, elegimos una nueva dirección aleatoria
	if (TiempoParaCambiarRuta <= 0.0f)
	{
		// Dirección aleatoria en X e Y (Z es 0 para que no mire al cielo)
		DireccionMerodeo = FVector(FMath::RandRange(-1.f, 1.f), FMath::RandRange(-1.f, 1.f), 0.0f).GetSafeNormal();

		// Esperar entre 2 y 5 segundos antes de volver a girar
		TiempoParaCambiarRuta = FMath::RandRange(2.0f, 5.0f);
	}

	// Movemos al robot en esa dirección aleatoria
	AddMovementInput(DireccionMerodeo, 1.0f);
}