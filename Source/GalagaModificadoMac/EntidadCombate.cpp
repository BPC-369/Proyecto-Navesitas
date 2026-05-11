#include "EntidadCombate.h"
#include "NaveEnemigoAereo.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "Blueprint/UserWidget.h"

AEntidadCombate::AEntidadCombate()
{
	PrimaryActorTick.bCanEverTick = true;

	VidaMaxima = 100.0f; // La Vida Maxima que tendran por defecto todos sus hijos
	VidaActual = VidaMaxima; // Iniciamos con la vida llena
	EscudoMaximo = 0.0f; // Por defecto en 0 para naves comunes (Nave_CMN, etc.)
	EscudoActual = EscudoMaximo;

	// 1. SOLUCIÓN AL CRASH: Creamos un componente raíz para el Pawn
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ComponenteRaiz"));

	// 2. Creamos el componente de la UI
	WidgetBarraVida = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetBarraVida"));

	// 3. Ahora sí podemos pegarlo, porque RootComponent ya existe
	WidgetBarraVida->SetupAttachment(RootComponent);

	// 4. Lo subimos en el eje Z
	WidgetBarraVida->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));

	// 5. El secreto mágico para que mire a la cámara
	WidgetBarraVida->SetWidgetSpace(EWidgetSpace::World);
    
    // TRUCO EXTRA: Indicamos que por defecto la barra dibujará en pantalla
    WidgetBarraVida->SetDrawSize(FVector2D(150.0f, 20.0f)); // 150px de ancho, 20px de alto
}

// Called when the game starts or when spawned
void AEntidadCombate::BeginPlay()
{
	Super::BeginPlay();
	
	VidaActual = VidaMaxima;
	EscudoActual = EscudoMaximo;
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

	if(DamageCauser != nullptr){
		AActor* Tirador = DamageCauser->GetOwner(); // obtenemos el actor que disparo la bala

		if (Tirador != nullptr) // Vrificamos que exista el actor que disparo la bala
		{
			// Se intenta cambiar el actor a una entidad de combate (EntidadCombate*(clase Padre ed todos los enemigos))
			AEntidadCombate* TiradorEntidad = Cast<AEntidadCombate>(Tirador);

			// Si el actor existe y su faccion es exactamente igual a la nuestra, entonces es un aliado y no se aplicara daño.
			if (TiradorEntidad == nullptr)
			{
				TiradorEntidad = Cast<AEntidadCombate>(DamageCauser->GetOwner());
			}

			// 3. Ahora comprobamos el fuego amigo con el atacante real
			if (TiradorEntidad != nullptr && TiradorEntidad->Faccion == this->Faccion)
			{
				return 0.0f; // Bloquear daño aliado
			}
		}
	}
	if (EscudoActual > 0.0f)
	{
		float DanioSobrante = DanioReal - EscudoActual;

		if (DanioSobrante > 0.0f)
		{
			// El escudo se rompe y el resto pasa a la vida
			EscudoActual = 0.0f;
			VidaActual -= DanioSobrante;
		}
		else
		{
			// El escudo es fuerte y absorbe todo el impacto
			EscudoActual -= DanioReal;
		}
	}
	else
	{
		// Si no hay escudo, el daño va directo a la vida
		VidaActual -= DanioReal;
	}

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

float AEntidadCombate::ObtenerPorcentajeVida() const
{
	if (VidaMaxima <= 0.0f) return 0.0f;
	return VidaActual / VidaMaxima;
}

float AEntidadCombate::ObtenerPorcentajeEscudo() const
{
	if (EscudoMaximo <= 0.0f) return 0.0f;
	return EscudoActual / EscudoMaximo;
}