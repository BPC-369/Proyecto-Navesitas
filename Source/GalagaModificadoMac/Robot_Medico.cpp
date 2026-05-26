#include "Robot_Medico.h"
#include "ComponenteCombate.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ARobot_Medico::ARobot_Medico()
{
	bEsElite = false;
	RangoCuracionArea = 600.0f;
	TasaCuracionActual = 1.0f;
	bEstaCurando = false;
	AliadoObjetivo = nullptr;
	bTieneDestinoAleatorio = false;

	// Hacemos que sea más veloz que el enemigo terrestre normal (que era 300)
	VelocidadMovimiento = 500.0f;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCono(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Wedge_A.Shape_Wedge_A'"));
	if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(FormaCono.Object);

		// Rotamos el cono para que la punta mire hacia el frente (Eje X)
		MallaEnemiga->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
}

void ARobot_Medico::BeginPlay()
{
	Super::BeginPlay();

	// Configuramos sus stats de combate (Débil y frágil)
	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 25.0f; // Menos vida que los 50.0f normales
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
	}

	// Iniciamos el radar de aliados (busca cada 1 segundo en vez de cada frame)
	GetWorldTimerManager().SetTimer(TimerBusqueda, this, &ARobot_Medico::BuscarAliado, 1.0f, true);
}

void ARobot_Medico::ActualizarComportamiento()
{
	// Si está curando, se queda quieto, no hacemos nada de movimiento
	if (bEstaCurando) return;

	// Si tenemos un objetivo válido y no ha muerto
	if (AliadoObjetivo != nullptr && IsValid(AliadoObjetivo))
	{
		// 1. NI BIEN SIENTA AL ALIADO: Interrumpimos el paseo aleatorio
		bTieneDestinoAleatorio = false;
		GetWorldTimerManager().ClearTimer(TimerPaseo);

		UComponenteCombate* CompAliado = AliadoObjetivo->FindComponentByClass<UComponenteCombate>();

		if (CompAliado && CompAliado->VidaActual > 0.0f)
		{
			float Distancia = FVector::Dist(GetActorLocation(), AliadoObjetivo->GetActorLocation());

			// Si estamos lo suficientemente cerca, lo agarramos y empezamos a curar
			if (Distancia <= RangoAtaque) // Usamos tu variable RangoAtaque como rango de curación
			{
				IniciarCuracion();
			}
			else
			{
				// Nos movemos hacia el aliado usando tu mismo sistema de movimiento
				FVector DireccionHaciaAliado = AliadoObjetivo->GetActorLocation() - GetActorLocation();
				DireccionHaciaAliado.Z = 0.0f;
				DireccionHaciaAliado.Normalize();

				AddMovementInput(DireccionHaciaAliado, 1.0f);
			}
		}
		else
		{
			// El aliado murió o ya no es válido, soltamos el objetivo
			AliadoObjetivo = nullptr;
		}
	}
	else
	{
		// 2. MODO PATRULLA: No hay aliados heridos, se mueve al azar
		if (!bTieneDestinoAleatorio)
		{
			GenerarDestinoAleatorio();

			// Si se choca con una pared y no llega, a los 4 segundos genera otro punto para no quedarse atascado
			GetWorldTimerManager().SetTimer(TimerPaseo, this, &ARobot_Medico::GenerarDestinoAleatorio, 4.0f, true);
		}

		float DistanciaAlDestino = FVector::Dist(GetActorLocation(), DestinoAleatorio);

		if (DistanciaAlDestino > 100.0f)
		{
			// Camina hacia el punto aleatorio
			FVector DireccionHaciaDestino = DestinoAleatorio - GetActorLocation();
			DireccionHaciaDestino.Z = 0.0f; // Ignoramos Z para que no intente volar ni hundirse
			DireccionHaciaDestino.Normalize();

			// Le ponemos 0.5f para que camine más lento y relajado cuando está patrullando
			AddMovementInput(DireccionHaciaDestino, 0.5f);
		}
		else
		{
			// Llegó a su destino aleatorio, pedimos que genere otro
			bTieneDestinoAleatorio = false;
		}
	}
}

void ARobot_Medico::BuscarAliado()
{
	if (bEstaCurando) return;

	TArray<AActor*> EnemigosEnMundo;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemigoTerrestre::StaticClass(), EnemigosEnMundo);

	AEnemigoTerrestre* AliadoMasCercano = nullptr;
	float DistanciaMinima = MAX_FLT;

	for (AActor* Actor : EnemigosEnMundo)
	{
		AEnemigoTerrestre* PosibleAliado = Cast<AEnemigoTerrestre>(Actor);

		if (PosibleAliado && PosibleAliado != this)
		{
			// Buscamos su chip de combate
			UComponenteCombate* CompPosible = PosibleAliado->FindComponentByClass<UComponenteCombate>();

			if (CompPosible && CompPosible->Faccion == "Enemigo")
			{
				float PorcentajeVida = CompPosible->ObtenerPorcentajeVida();

				// Si tiene menos del 70% de vida y sigue vivo
				if (PorcentajeVida < 0.7f && CompPosible->VidaActual > 0.0f)
				{
					float Distancia = FVector::Dist(GetActorLocation(), PosibleAliado->GetActorLocation());
					if (Distancia < DistanciaMinima)
					{
						DistanciaMinima = Distancia;
						AliadoMasCercano = PosibleAliado;
					}
				}
			}
		}
	}

	AliadoObjetivo = AliadoMasCercano;
}

void ARobot_Medico::IniciarCuracion()
{
	if (!AliadoObjetivo) return;

	bEstaCurando = true;

	// Inmovilizamos al aliado usando el componente de movimiento nativo de Unreal
	if (AliadoObjetivo->GetCharacterMovement())
	{
		AliadoObjetivo->GetCharacterMovement()->DisableMovement();
	}

	// Si es Elite, soltamos la ráfaga de curación en área primero
	if (bEsElite)
	{
		CurarAreaElite();
	}

	TasaCuracionActual = 1.0f;

	// Ejecutamos la curación gradual 1 vez por segundo
	GetWorldTimerManager().SetTimer(TimerCuracion, this, &ARobot_Medico::CuracionGradual, 1.0f, true);
}

void ARobot_Medico::CuracionGradual()
{
	if (!IsValid(AliadoObjetivo))
	{
		DetenerCuracion();
		return;
	}

	UComponenteCombate* CompAliado = AliadoObjetivo->FindComponentByClass<UComponenteCombate>();
	if (!CompAliado || CompAliado->VidaActual <= 0.0f)
	{
		DetenerCuracion();
		return;
	}

	// Curamos al aliado
	CompAliado->VidaActual = FMath::Clamp(CompAliado->VidaActual + TasaCuracionActual, 0.0f, CompAliado->VidaMaxima);

	// Si ya está full vida, paramos
	if (CompAliado->VidaActual >= CompAliado->VidaMaxima)
	{
		DetenerCuracion();
		return;
	}

	// Aumentamos la tasa gradualmente, con un tope de 10
	TasaCuracionActual = FMath::Min(TasaCuracionActual + 1.0f, 10.0f);
}

void ARobot_Medico::DetenerCuracion()
{
	bEstaCurando = false;

	// Le devolvemos el movimiento al aliado (si sigue vivo)
	if (IsValid(AliadoObjetivo) && AliadoObjetivo->GetCharacterMovement())
	{
		AliadoObjetivo->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	AliadoObjetivo = nullptr;
	GetWorldTimerManager().ClearTimer(TimerCuracion);
}

void ARobot_Medico::CurarAreaElite()
{
	TArray<AActor*> EnemigosEnArea;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemigoTerrestre::StaticClass(), EnemigosEnArea);

	for (AActor* Actor : EnemigosEnArea)
	{
		AEnemigoTerrestre* AliadoArea = Cast<AEnemigoTerrestre>(Actor);
		if (AliadoArea)
		{
			float Distancia = FVector::Dist(GetActorLocation(), AliadoArea->GetActorLocation());
			if (Distancia <= RangoCuracionArea)
			{
				UComponenteCombate* CompArea = AliadoArea->FindComponentByClass<UComponenteCombate>();
				if (CompArea && CompArea->Faccion == "Enemigo" && CompArea->VidaActual > 0.0f)
				{
					// Cura 30% de SU vida máxima
					float CuracionBurst = CompArea->VidaMaxima * 0.30f;
					CompArea->VidaActual = FMath::Clamp(CompArea->VidaActual + CuracionBurst, 0.0f, CompArea->VidaMaxima);
				}
			}
		}
	}
}

void ARobot_Medico::Destroyed()
{
	// Si nos matan mientras curamos, soltamos al aliado para que pueda moverse
	if (bEstaCurando)
	{
		DetenerCuracion();
	}

	GetWorldTimerManager().ClearTimer(TimerBusqueda);

	Super::Destroyed();
}

void ARobot_Medico::GenerarDestinoAleatorio()
{
	// Genera un punto al azar en un radio de 800 unidades
	float RadioPaseo = 800.0f;
	FVector OffsetAleatorio = FVector(FMath::RandRange(-RadioPaseo, RadioPaseo), FMath::RandRange(-RadioPaseo, RadioPaseo), 0.0f);

	DestinoAleatorio = GetActorLocation() + OffsetAleatorio;
	bTieneDestinoAleatorio = true;
}