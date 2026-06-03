//// Fill out your copyright notice in the Description page of Project Settings.

#include "NaveLider.h"
#include "BalaEspecial.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h" // <-- AÑADIDO: Necesario para manejar las colisiones
#include "ComponenteCombate.h"
#include "GalagaModificadoMacProjectile.h" // Asegúrate de incluir tu clase de proyectil

ANaveLider::ANaveLider()
{
	PrimaryActorTick.bCanEverTick = true;
	ContadorDisparos = 0;

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->EscudoMaximo = 0.0f;
		ComponenteCombate->EscudoActual = ComponenteCombate->EscudoMaximo;
		ComponenteCombate->Faccion = FName("Enemigo");
	}
	
	// Buscamos la malla de la Nave Comando
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow01.StarSparrow01'"));

	if (MeshAsset.Succeeded())
	{
		// MallaEnemiga es el componente que heredas de NaveEnemigoAereo
		MallaEnemiga->SetStaticMesh(MeshAsset.Object);

		// Opcional: Si quieres que sea más grande que las comunes para que imponga
		MallaEnemiga->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
		MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void ANaveLider::BeginPlay()
{
	Super::BeginPlay();
	// Iniciamos el bucle de ataque automático 
	GetWorldTimerManager().SetTimer(TimerAtaque, this, &ANaveLider::Atacar, FrecuenciaAtaque, true);
}

void ANaveLider::Tick(float DeltaTime)
{
	//Super::Tick llama a Volar() que está en NaveEnemigoAereo.cpp 
	Super::Tick(DeltaTime);

	// Aquí podrías poner lógica que solo el Líder haga cada frame,
	// como regenerar un poco de escudo.
}

void ANaveLider::Atacar()
{
	UWorld* const World = GetWorld();

	// Validamos que el mundo exista
	if (World != nullptr)
	{
		//Tomamos la rotación actual de la nave para que la bala salga disparada hacia adelante.
		const FRotator RotacionDisparo = GetActorRotation();

		//Colocamos la Bala un poco por delante de la nave
		const FVector PosicionDisparo = GetActorLocation() + (GetActorForwardVector() * 180.0f);

		// indicamos que esta nave es la dueña de la bala que va a nacer
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// CORRECCIÓN: Guardamos el proyectil al nacer y le decimos que ignore la colisión con el Líder
		AGalagaModificadoMacProjectile* ProyectilLider = World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparo, RotacionDisparo, SpawnParams);

		if (ProyectilLider)
		{
			UPrimitiveComponent* ColliderProyectil = ProyectilLider->FindComponentByClass<UPrimitiveComponent>();
			if (ColliderProyectil)
			{
				ColliderProyectil->IgnoreActorWhenMoving(this, true);
			}
		}
	};
	
	/*
	// NOTA: Tu código comentado para disparar balas especiales está muy bien planteado. 
	// Si más adelante lo descomentas, recuerda aplicar esta misma lógica de 
	// FindComponentByClass<UPrimitiveComponent>() -> IgnoreActorWhenMoving(this, true) 
	// a la "ABalaEspecial" para que tampoco choque con la nave.
	*/
}
	/*ContadorDisparos++;

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator SpawnRotation = GetActorRotation();

	if (ContadorDisparos % 10 == 0) // Cada 10 disparos usa la bala especial
	{
		GetWorld()->SpawnActor<ABalaEspecial>(ABalaEspecial::StaticClass(), SpawnLocation, SpawnRotation);
	}
	else
	{
		// Disparo normal heredado de la clase base de proyectiles
		GetWorld()->SpawnActor<AGalagaModificadoMacProjectile>(AGalagaModificadoMacProjectile::StaticClass(), SpawnLocation, SpawnRotation);
	}*/



// ajustamos los daños de impacto Primero Escudo, luego VidaLider

/*
float ANaveLider::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// 1. Primero se agota el escudo 
	if (EnergiaEscudo > 0)
	{
		EnergiaEscudo -= DamageAmount;

		if (EnergiaEscudo < 0) EnergiaEscudo = 0;

		UE_LOG(LogTemp, Warning, TEXT("LIDER: Daño al Escudo. Restante: %f"), EnergiaEscudo);
		return 0.0f; // El escudo absorbió el impacto
	}

	// 2. Cuando el escudo es 0, restamos de la vida real (VidaLider)
	VidaLider -= DamageAmount;
	UE_LOG(LogTemp, Error, TEXT("LIDER: ¡Escudo caído! Vida restante: %f"), VidaLider);

	if (VidaLider <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LIDER ELIMINADO"));
		GetWorldTimerManager().ClearTimer(TimerAtaque);
		Destroy(); // Se elimina del mundo
	}

	return DamageAmount;
}
*/