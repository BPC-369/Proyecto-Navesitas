#include "NaveKamikase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GalagaModificadoMacPawn.h"
#include "ComponenteCombate.h"

ANaveKamikase::ANaveKamikase() {
	Velocidad = 700.0f;
    RadioExplosion = 1500.0f;
    DanoExplosion = 1500.0f;
	
	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->EscudoMaximo = 0.0f;
		ComponenteCombate->EscudoActual = ComponenteCombate->EscudoMaximo;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Forma(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow15.StarSparrow15'"));
	if (Forma.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(Forma.Object);
		MallaEnemiga->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
		MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void ANaveKamikase::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Nos aseguramos de que chocamos contra un actor válido y que no sea consigo misma
	if (Other != nullptr && Other != this)
	{
		// 2. Intentamos transformar el actor genérico en tu clase específica
		// Reemplaza 'ATuClaseDelJugador' por el nombre real de tu clase (ej. ANaveJugador)
		UComponenteCombate* CompGolpeado = Other->FindComponentByClass<UComponenteCombate>();

		// 3. Si el casteo es exitoso (no es nulo), significa que sí chocamos con una nave
		if (CompGolpeado != nullptr && ComponenteCombate != nullptr)
		{
			// 4. ...comparamos la facción del chip golpeado con nuestra propia facción.
			// Si NO son iguales (es decir, si no es otro "Enemigo"), ¡explotamos!
			if (CompGolpeado->Faccion != ComponenteCombate->Faccion)
			{
				Explotar();
			}
		}
	}
}

void ANaveKamikase::Explotar()
{
	// Obtenemos el punto exacto donde está la nave en este momento
	FVector Ubicacion = GetActorLocation();

	// 3. Crear el Daño de Área (AoE)
	TArray<AActor*> IgnorarActores;
	IgnorarActores.Add(this); // Para que la nave no se intente dañar a sí misma antes de borrarse

	// Esta función dibuja una esfera invisible y daña todo lo que toque dentro del radio
	UGameplayStatics::ApplyRadialDamage(
		this,                        // El mundo actual
		DanoExplosion,               // Cantidad de daño
		Ubicacion,                   // Centro de la explosión
		RadioExplosion,              // Radio de la explosión 
		UDamageType::StaticClass(),  // Clase de daño 
		IgnorarActores,              // Lista de actores a ignorar
		this,                        // Actor causante del daño
		nullptr                      // Controlador (puede ir nulo)
	);

	Destroy();
}

void ANaveKamikase::Atacar()
{

}