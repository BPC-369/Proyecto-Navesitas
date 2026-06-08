#include "Robot_Medico.h"
#include "ComponenteCombate.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ARobot_Medico::ARobot_Medico()
{
	bEsElite = false;
	RangoCuracionArea = 600.0f;
	TasaCuracionActual = 5.0f; // Empieza en 5
	bEstaCurando = false;
	AliadoObjetivo = nullptr;
	bTieneDestinoAleatorio = false;

	// Velocidad máxima (Correr/Huir)
	VelocidadMovimiento = 500.0f;
	RangoDeteccion = 3000.0f; // Necesita un radar grande para ver aliados y al jugador
	RangoAtaque = 200.0f; // Distancia a la que tiene que estar para empezar a curar

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FormaCono(TEXT("SkeletalMesh'/Game/Geometry/robotMedico/mallaMedico.mallaMedico'"));
	if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
	{
		GetMesh()->SetSkeletalMesh(FormaCono.Object);
		GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimacionRobotAsset(TEXT("AnimBlueprint'/Game/Blueprints/ABP_Medico.ABP_Medico_C'"));

	if (AnimacionRobotAsset.Succeeded())
	{
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		GetMesh()->SetAnimInstanceClass(AnimacionRobotAsset.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageCurarAsset(TEXT("AnimMontage'/Game/Geometry/robotMedico/Curar_Montage.Curar_Montage'"));

	if (MontageCurarAsset.Succeeded())
	{
		AnimacionCurar = MontageCurarAsset.Object;
	}
}

void ARobot_Medico::BeginPlay()
{
	Super::BeginPlay();

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 25.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
	}

	GetWorldTimerManager().SetTimer(TimerBusqueda, this, &ARobot_Medico::BuscarAliado, 1.0f, true);
}

void ARobot_Medico::ActualizarComportamiento()
{
	// Si está curando, se queda estático
	if (bEstaCurando) return;

	APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// 1. MODO SALVADOR: Tiene un objetivo herido
	if (AliadoObjetivo != nullptr && IsValid(AliadoObjetivo))
	{
		bTieneDestinoAleatorio = false;
		GetWorldTimerManager().ClearTimer(TimerPaseo);

		UComponenteCombate* CompAliado = AliadoObjetivo->FindComponentByClass<UComponenteCombate>();

		if (CompAliado && CompAliado->VidaActual > 0.0f && CompAliado->VidaActual < CompAliado->VidaMaxima)
		{
			float Distancia = FVector::Dist(GetActorLocation(), AliadoObjetivo->GetActorLocation());

			if (Distancia <= RangoAtaque)
			{
				IniciarCuracion();
			}
			else
			{
				// Corre directo a salvarlo
				GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento; // 500 (Corre)
				FVector DireccionHaciaAliado = AliadoObjetivo->GetActorLocation() - GetActorLocation();
				DireccionHaciaAliado.Z = 0.0f;
				AddMovementInput(DireccionHaciaAliado.GetSafeNormal(), 1.0f);
			}
		}
		else
		{
			// El aliado murió o ya está full vida
			AliadoObjetivo = nullptr;
		}
		return; // Salimos de la función para que no haga la fase de escape
	}

	// 2. MODO TÁCTICO: No hay heridos. Evaluamos la amenaza del jugador.
	float DistanciaAlJugador = Jugador ? FVector::Dist(GetActorLocation(), Jugador->GetActorLocation()) : MAX_FLT;

	if (Jugador && DistanciaAlJugador <= RangoDeteccion)
	{
		bTieneDestinoAleatorio = false;

		AEnemigoTerrestre* EscudoCarne = ObtenerEscudoDeCarneMasCercano();

		if (EscudoCarne)
		{
			// Si estás cerca, corre despavorido (500). Si estás lejos, va al trote táctico (350).
			if (DistanciaAlJugador < 600.0f)
			{
				GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;
			}
			else
			{
				GetCharacterMovement()->MaxWalkSpeed = 350.0f;
			}

			// GEOMETRÍA: El punto seguro es 400 unidades DETRÁS de su aliado (opuesto a ti)
			FVector PosicionEscudo = EscudoCarne->GetActorLocation();
			FVector PosicionJugador = Jugador->GetActorLocation();

			FVector DirJugadorAEscudo = (PosicionEscudo - PosicionJugador).GetSafeNormal();
			DirJugadorAEscudo.Z = 0.0f;

			FVector PuntoRetaguardia = PosicionEscudo + (DirJugadorAEscudo * 400.0f);
			FVector DirHaciaRetaguardia = (PuntoRetaguardia - GetActorLocation()).GetSafeNormal();
			DirHaciaRetaguardia.Z = 0.0f;

			// TRUCO DE ESQUIVE: Si el jugador está DEMASIADO cerca del médico, 
			// mezclamos su deseo de ir a la retaguardia con el instinto de alejarse de ti.
			// Así evita correr directamente hacia ti si el aliado quedó a tus espaldas.
			if (DistanciaAlJugador < 350.0f)
			{
				FVector DirEscapeJugador = (GetActorLocation() - PosicionJugador).GetSafeNormal();
				DirEscapeJugador.Z = 0.0f;
				// 60% huir de ti, 40% ir a la retaguardia
				DirHaciaRetaguardia = (DirEscapeJugador * 0.6f + DirHaciaRetaguardia * 0.4f).GetSafeNormal();
			}

			// Se mueve hacia el punto seguro
			if (FVector::Dist(GetActorLocation(), PuntoRetaguardia) > 100.0f)
			{
				AddMovementInput(DirHaciaRetaguardia, 1.0f);
			}
			else
			{
				// Si ya llegó a la espalda de su aliado, se da la vuelta para mirarte de frente
				FVector DirMirar = (PosicionJugador - GetActorLocation()).GetSafeNormal();
				DirMirar.Z = 0.0f;
				SetActorRotation(DirMirar.Rotation());
			}
		}
		else
		{
			// PÁNICO PURO: Todos sus aliados de combate murieron. Huye de ti en línea recta.
			GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;
			FVector DirEscape = (GetActorLocation() - Jugador->GetActorLocation()).GetSafeNormal();
			DirEscape.Z = 0.0f;
			AddMovementInput(DirEscape, 1.0f);
		}
	}
	else
	{
		// 3. MODO PATRULLA: No hay heridos y el jugador está lejos
		GetCharacterMovement()->MaxWalkSpeed = 150.0f; // Camina lento

		if (!bTieneDestinoAleatorio)
		{
			GenerarDestinoAleatorio();
			GetWorldTimerManager().SetTimer(TimerPaseo, this, &ARobot_Medico::GenerarDestinoAleatorio, 4.0f, true);
		}

		float DistanciaAlDestino = FVector::Dist(GetActorLocation(), DestinoAleatorio);
		if (DistanciaAlDestino > 100.0f)
		{
			FVector DireccionHaciaDestino = (DestinoAleatorio - GetActorLocation()).GetSafeNormal();
			DireccionHaciaDestino.Z = 0.0f;
			AddMovementInput(DireccionHaciaDestino, 1.0f);
		}
		else
		{
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

		// No se cura a sí mismo y debe estar dentro del rango de detección visual
		if (PosibleAliado && PosibleAliado != this && FVector::Dist(GetActorLocation(), PosibleAliado->GetActorLocation()) <= RangoDeteccion)
		{
			UComponenteCombate* CompPosible = PosibleAliado->FindComponentByClass<UComponenteCombate>();

			if (CompPosible && CompPosible->Faccion == "Enemigo")
			{
				float PorcentajeVida = CompPosible->ObtenerPorcentajeVida();

				// Si tiene 70% o menos de vida
				if (PorcentajeVida <= 0.7f && CompPosible->VidaActual > 0.0f)
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

// Nueva función para encontrar a alguien detrás de quien esconderse
AEnemigoTerrestre* ARobot_Medico::ObtenerEscudoDeCarneMasCercano()
{
	TArray<AActor*> EnemigosEnMundo;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemigoTerrestre::StaticClass(), EnemigosEnMundo);

	AEnemigoTerrestre* AliadoSanoCercano = nullptr;
	float DistanciaMinima = 2000.0f;

	for (AActor* Actor : EnemigosEnMundo)
	{
		AEnemigoTerrestre* PosibleAliado = Cast<AEnemigoTerrestre>(Actor);

		if (PosibleAliado && PosibleAliado != this && !PosibleAliado->IsA(ARobot_Medico::StaticClass()))
		{
			UComponenteCombate* CompPosible = PosibleAliado->FindComponentByClass<UComponenteCombate>();

			if (CompPosible && CompPosible->Faccion == "Enemigo" && CompPosible->VidaActual > 0.0f)
			{
				float Distancia = FVector::Dist(GetActorLocation(), PosibleAliado->GetActorLocation());
				if (Distancia < DistanciaMinima)
				{
					DistanciaMinima = Distancia;
					AliadoSanoCercano = PosibleAliado;
				}
			}
		}
	}
	return AliadoSanoCercano;
}

void ARobot_Medico::IniciarCuracion()
{
	if (!AliadoObjetivo) return;

	bEstaCurando = true;
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;

	if (AnimacionCurar != nullptr)
	{
		PlayAnimMontage(AnimacionCurar);
	}

	if (AliadoObjetivo->GetCharacterMovement())
	{
		// 1. LE AVISAMOS AL ABUELO QUE NO LO MUEVA
		AliadoObjetivo->bEstaSiendoCurado = true;

		VelocidadOriginalAliado = AliadoObjetivo->GetCharacterMovement()->MaxWalkSpeed;
		AliadoObjetivo->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	}

	if (bEsElite)
	{
		CurarAreaElite();
	}

	TasaCuracionActual = 5.0f;
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

	CompAliado->VidaActual = FMath::Clamp(CompAliado->VidaActual + TasaCuracionActual, 0.0f, CompAliado->VidaMaxima);

	// Progresión de curación de 5 en 5, hasta un máximo de 50 por segundo
	TasaCuracionActual = FMath::Min(TasaCuracionActual + 5.0f, 50.0f);

	if (CompAliado->VidaActual >= CompAliado->VidaMaxima)
	{
		DetenerCuracion();
	}
}

void ARobot_Medico::DetenerCuracion()
{
	bEstaCurando = false;
	GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;

	if (IsValid(AliadoObjetivo))
	{
		// 2. TERMINÓ LA CURACIÓN. LE QUITAMOS LA ETIQUETA AL ABUELO
		AliadoObjetivo->bEstaSiendoCurado = false;

		if (AliadoObjetivo->GetCharacterMovement())
		{
			AliadoObjetivo->GetCharacterMovement()->MaxWalkSpeed = VelocidadOriginalAliado;
		}
	}

	StopAnimMontage(AnimacionCurar);
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
					float CuracionBurst = CompArea->VidaMaxima * 0.30f;
					CompArea->VidaActual = FMath::Clamp(CompArea->VidaActual + CuracionBurst, 0.0f, CompArea->VidaMaxima);
				}
			}
		}
	}
}

void ARobot_Medico::Destroyed()
{
	if (bEstaCurando)
	{
		DetenerCuracion();
	}

	GetWorldTimerManager().ClearTimer(TimerBusqueda);
	GetWorldTimerManager().ClearTimer(TimerPaseo);

	Super::Destroyed();
}

void ARobot_Medico::GenerarDestinoAleatorio()
{
	float RadioPaseo = 800.0f;
	FVector OffsetAleatorio = FVector(FMath::RandRange(-RadioPaseo, RadioPaseo), FMath::RandRange(-RadioPaseo, RadioPaseo), 0.0f);

	DestinoAleatorio = GetActorLocation() + OffsetAleatorio;
	bTieneDestinoAleatorio = true;
}