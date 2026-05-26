#include "RobotFrancotirador.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h" // Para dibujar el láser de la bala
#include "ComponenteCombate.h"

ARobotFrancotirador::ARobotFrancotirador()
{
	PrimaryActorTick.bCanEverTick = true;

	// --- CONFIGURACIÓN ESTADÍSTICAS EXTREMAS ---
	RangoDeteccion = 3000.0f;     // RANGO 1: Detección amplísima. Lo busca y se acerca.
	RangoPreparacion = 2000.0f;   // RANGO 2: Se detiene y prepara el arma.
	RangoDisparo = 1500.0f;       // RANGO 3: Abre fuego.
	DanioBase = 45.0f;            // Daño letal característico de un francotirador.
	VelocidadMovimiento = 200.0f; // Se mueve un poco más lento por el peso del arma.

	TiempoEntreDisparos = 2.5f;   // Cadencia lenta
	TiempoUltimoDisparo = 0.0f;

	// --- ASIGNACIÓN DE MALLA EN C++ PURO ---
	// Cargamos un cubo básico del motor ya que no podemos usar Blueprints
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaFrancotirador(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (MallaFrancotirador.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(MallaFrancotirador.Object);
		// Escalamos el cubo para que parezca más alto y delgado, como un francotirador
		MallaEnemiga->SetWorldScale3D(FVector(0.5f, 0.5f, 1.8f));
	}

	// Sobrescribir vida
	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 30.0f; // Un francotirador tiene poca salud
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
	}
}

void ARobotFrancotirador::ActualizarComportamiento()
{
	float Distancia = CalcularDistanciaAlJugador();

	// Si no encuentra al jugador, o está fuera del alcance máximo, no hace nada
	if (Distancia < 0.0f || Distancia > RangoDeteccion) return;

	// LÓGICA DE LOS 3 RANGOS
	if (Distancia <= RangoDisparo)
	{
		// --- RANGO 3 (Disparo) ---
		// Se detiene (no llamamos a Mover()), apunta y dispara según la cadencia
		ApuntarAlJugador();

		float TiempoActual = GetWorld()->GetTimeSeconds();
		if (TiempoActual - TiempoUltimoDisparo >= TiempoEntreDisparos)
		{
			Atacar();
			TiempoUltimoDisparo = TiempoActual;
		}
	}
	else if (Distancia <= RangoPreparacion)
	{
		// --- RANGO 2 (Preparación) ---
		// Está lo suficientemente cerca para prepararse, pero no para disparar.
		// Se detiene y sigue al jugador con la mirada.
		ApuntarAlJugador();
	}
	else if (Distancia <= RangoDeteccion)
	{
		// --- RANGO 1 (Acercamiento) ---
		// Aún está lejos del rango de preparación. Se mueve hacia el jugador.
		Mover();
	}
}

void ARobotFrancotirador::ApuntarAlJugador()
{
	APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Jugador)
	{
		FVector DireccionHaciaJugador = Jugador->GetActorLocation() - GetActorLocation();
		DireccionHaciaJugador.Z = 0.0f; // Evitar que el robot rote hacia arriba o abajo
		SetActorRotation(DireccionHaciaJugador.Rotation());
	}
}

void ARobotFrancotirador::Atacar()
{
	APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Jugador) return;

	// Lógica de "Bala de Francotirador" usando LineTrace (Hitscan)
	FVector Inicio = GetActorLocation();
	FVector Fin = Jugador->GetActorLocation();

	FHitResult ResultadoImpacto;
	FCollisionQueryParams ParametrosColision;
	ParametrosColision.AddIgnoredActor(this); // Que no se dispare a sí mismo

	bool bImpacto = GetWorld()->LineTraceSingleByChannel(ResultadoImpacto, Inicio, Fin, ECC_Visibility, ParametrosColision);

	// Efecto visual en C++ puro: Dibuja una línea roja láser temporal de 0.5 segundos
	DrawDebugLine(GetWorld(), Inicio, Fin, FColor::Red, false, 0.5f, 0, 3.0f);

	if (bImpacto)
	{
		AActor* ActorGolpeado = ResultadoImpacto.GetActor();
		if (ActorGolpeado && ActorGolpeado == Jugador)
		{
			// Si el láser impacta al jugador, aplicar daño
			UGameplayStatics::ApplyDamage(ActorGolpeado, DanioBase, GetController(), this, UDamageType::StaticClass());
			UE_LOG(LogTemp, Warning, TEXT("¡Francotirador Acertó! Daño infligido: %f"), DanioBase);
		}
	}
}