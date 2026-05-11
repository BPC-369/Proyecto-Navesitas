#include "Robot_Razo.h"
#include "ComponenteCombate.h"
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/CharacterMovementComponent.h" // Vital para reducir la velocidad física

ARobot_Razo::ARobot_Razo()
{
	PrimaryActorTick.bCanEverTick = true;

	VelocidadMovimiento = 250.0f;
	RangoDeteccion = 800.0f;
	DanioBase = 15.0f;
	RangoAtaque = 150.0f;

	bProtocoloFuriaActivado = false;
	bEstaAtacando = false; // Iniciamos sin atacar

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 100.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}
}

void ARobot_Razo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ComponenteCombate != nullptr)
	{
		if (!bProtocoloFuriaActivado && ComponenteCombate->VidaActual <= (ComponenteCombate->VidaMaxima * 0.5f))
		{
			ComponenteCombate->VidaActual += 20.0f;

			if (ComponenteCombate->VidaActual > ComponenteCombate->VidaMaxima)
			{
				ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
			}

			DanioBase *= 1.5f;
			bProtocoloFuriaActivado = true;
		}
	}
}

void ARobot_Razo::Atacar()
{
	// 1. Si ya estamos en medio de una canalización, ignoramos la orden para no reiniciar el timer
	if (bEstaAtacando) return;

	bEstaAtacando = true;

	// 2. Reducimos la velocidad física a la mitad mientras canaliza el golpe
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->MaxWalkSpeed = (VelocidadMovimiento / 2.0f);
	}

	// 3. Iniciamos el temporizador: Llama a "FinalizarAtaque" en exactamente 0.5 segundos. 
	// El 'false' indica que no se repetirá en bucle.
	GetWorldTimerManager().SetTimer(TimerHandle_Canalizacion, this, &ARobot_Razo::FinalizarAtaque, 0.5f, false);
}

void ARobot_Razo::FinalizarAtaque()
{
	// 1. Restauramos la velocidad física a su valor original
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;
	}

	// 2. Calculamos dónde está el jugador medio segundo después
	float DistanciaActual = CalcularDistanciaAlJugador();

	// 3. Si el jugador sigue dentro del rango de ataque (Le damos un margen extra de 20.0f por el hitbox)
	if (DistanciaActual > 0.0f && DistanciaActual <= (RangoAtaque + 20.0f))
	{
		APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (Jugador != nullptr)
		{
			// ¡El golpe conecta! Aplicamos el daño.
			UGameplayStatics::ApplyDamage(Jugador, DanioBase, GetController(), this, UDamageType::StaticClass());

			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("¡ROBOT RAZO: Golpe Conectado!"));
		}
	}
	else
	{
		// El jugador logró escapar del rango antes de los 0.5 segundos
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("¡ROBOT RAZO: Ataque Fallido!"));
	}

	// 4. Liberamos el seguro para que pueda volver a atacar en el futuro
	bEstaAtacando = false;
}