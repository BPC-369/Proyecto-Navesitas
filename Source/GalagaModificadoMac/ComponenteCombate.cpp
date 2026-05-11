#include "ComponenteCombate.h"
#include "GameFramework/Actor.h"

UComponenteCombate::UComponenteCombate()
{
	// Apagamos el Tick para ahorrar rendimiento, el componente solo reacciona cuando recibe daño
	PrimaryComponentTick.bCanEverTick = false;

	VidaMaxima = 100.0f;
	VidaActual = VidaMaxima;
	EscudoMaximo = 0.0f;
	EscudoActual = EscudoMaximo;
	Faccion = TEXT("Enemigo");
}

void UComponenteCombate::BeginPlay()
{
	Super::BeginPlay();

	VidaActual = VidaMaxima;
	EscudoActual = EscudoMaximo;
}

float UComponenteCombate::HacerDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Ya no llamamos a Super::TakeDamage, simplemente tomamos el daño que nos mandó el Actor
	float DanioReal = DamageAmount;

	if (VidaActual <= 0.0f) return 0.0f;

	// 1. Lógica de Fuego Amigo actualizada a la nueva arquitectura
	if (DamageCauser != nullptr)
	{
		AActor* Tirador = DamageCauser;

		// Si el que hizo daño fue un proyectil, buscamos a la nave que lo disparó
		if (DamageCauser->GetOwner() != nullptr)
		{
			Tirador = DamageCauser->GetOwner();
		}

		// La Magia: Le preguntamos al tirador "¿Tienes el chip ComponenteCombate instalado?"
		UComponenteCombate* CompTirador = Tirador->FindComponentByClass<UComponenteCombate>();

		// Si tiene el chip y pertenece a nuestra misma faccion, anulamos el daño
		if (CompTirador != nullptr && CompTirador->Faccion == this->Faccion)
		{
			return 0.0f;
		}
	}

	// 2. Matemática de Escudos (Intacta, tu lógica era perfecta)
	if (EscudoActual > 0.0f)
	{
		float DanioSobrante = DanioReal - EscudoActual;

		if (DanioSobrante > 0.0f)
		{
			EscudoActual = 0.0f;
			VidaActual -= DanioSobrante;
		}
		else
		{
			EscudoActual -= DanioReal;
		}
	}
	else
	{
		VidaActual -= DanioReal;
	}

	// 3. Verificación de Muerte
	if (VidaActual <= 0.0f)
	{
		VidaActual = 0.0f;
		Morir();
	}

	return DanioReal;
}

void UComponenteCombate::Morir()
{
	// Buscamos a la nave o robot dueño de este componente y lo destruimos
	AActor* Dueno = GetOwner();
	if (Dueno != nullptr)
	{
		Dueno->Destroy();
	}
}

float UComponenteCombate::ObtenerPorcentajeVida() const
{
	if (VidaMaxima <= 0.0f) return 0.0f;
	return VidaActual / VidaMaxima;
}

float UComponenteCombate::ObtenerPorcentajeEscudo() const
{
	if (EscudoMaximo <= 0.0f) return 0.0f;
	return EscudoActual / EscudoMaximo;
}