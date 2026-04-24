#include "EntidadCombate.h"
#include "NaveEnemigoAereo.h"

AEntidadCombate::AEntidadCombate()
{
	PrimaryActorTick.bCanEverTick = true;

	VidaMaxima = 100.0f; // La Vida Maxima que tendran por defecto todos sus hijos
	VidaActual = VidaMaxima; // Iniciamos con la vida llena
}

// Called when the game starts or when spawned
void AEntidadCombate::BeginPlay()
{
	Super::BeginPlay();
	
	VidaActual = VidaMaxima; // Aseguramos que empezamos con la vida llena al iniciar el juego
}

void AEntidadCombate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEntidadCombate::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEntidadCombate::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Obtenemos el daño real que se guardara una vez termine de ejecutarse esta funcion
	float DanioReal = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (VidaActual <= 0.0f) return 0.0f; // Si ya estamos muertos, no recibimos más daño.

	AActor* Tirador = DamageCauser->GetOwner(); // obtenemos el actor que disparo la bala

	if (Tirador != nullptr) // Vrificamos que exista el actor que disparo la bala
	{
		// Se intenta cambiar el actor a una entidad de combate (EntidadCombate*(clase Padre ed todos los enemigos))
		AEntidadCombate* TiradorEntidad = Cast<AEntidadCombate>(Tirador); 

		// Si el actor existe y su faccion es exactamente igual a la nuestra, entonces es un aliado y no se aplicara daño.
		if (TiradorEntidad != nullptr && TiradorEntidad->Faccion == this->Faccion) 
		{
			return 0.0f;
		}
	}

	// Restamos el daño a nuestra vida
	VidaActual -= DanioReal;

	// Verificamos si esta bala nos mató
	if (VidaActual <= 0.0f)
	{
		VidaActual = 0.0f;
		Morir();
	}

	return DanioReal; // 
}

void AEntidadCombate::Morir()
{
	Destroy();
}