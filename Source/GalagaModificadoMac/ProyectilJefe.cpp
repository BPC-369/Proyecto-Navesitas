/*#include "ProyectilJefe.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

AProyectilJefe::AProyectilJefe()
{
	// 1. Cambiar la malla visual
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile'"));
	DanoProyectil = 250.0f;
	if (MeshAsset.Succeeded())
	{
		// Usamos el nombre del componente que heredaste del padre (normalmente ProjectileMesh)
		// O lo buscamos de forma gen�rica:
		UStaticMeshComponent* MallaEncontrada = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

		if (MallaEncontrada)
		{
			MallaEncontrada->SetStaticMesh(MeshAsset.Object);
		}

		// Lo hacemos gigante para que se vea imponente
		SetActorScale3D(FVector(3.5f, 3.5f, 3.5f));
	}

	// 2. Cambiar la velocidad
	UProjectileMovementComponent* Movimiento = Cast<UProjectileMovementComponent>(GetComponentByClass(UProjectileMovementComponent::StaticClass()));

	if (Movimiento)
	{
		Movimiento->InitialSpeed = 1200.f;
		Movimiento->MaxSpeed = 1200.f;
	}
}*/

#include "ProyectilJefe.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AProyectilJefe::AProyectilJefe()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile'"));
	DanoProyectil = 250.0f; // Asignaci�n del usuario

	if (MeshAsset.Succeeded())
	{
		UStaticMeshComponent* MallaEncontrada = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

		if (MallaEncontrada)
		{
			MallaEncontrada->SetStaticMesh(MeshAsset.Object);

			// --- EL FILTRO DE "DIMENSI�N FANTASMA" (REVISADO) ---
			MallaEncontrada->SetCollisionResponseToAllChannels(ECR_Ignore); // Ignora todo
			MallaEncontrada->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // Solapa jugador

			// !!! REVISI�N IMPORTANTE !!!
			// 1. OBLIGATORIO: Activar la detecci�n de eventos de solapamiento.
			// Muchas mallas est�ticas lo traen desactivado por defecto.
			MallaEncontrada->SetGenerateOverlapEvents(true);

			// 2. CORRECCI�N: Lo regresamos a ECC_WorldDynamic.
			// Al usar explicitamente "IgnoreAll", es seguro. 
			// Si lo dejamos en WorldStatic, el motor a veces no procesa solapamientos con Pawns r�pidos.
			MallaEncontrada->SetCollisionObjectType(ECC_WorldDynamic);

			// 3. CONECTAR LA FUNCI�N (Mec�nica de Ingenier�a)
			// Le decimos a la malla: "Cuando empieces a solapar algo, llama a mi funci�n SobreSolapamientoMalla"
			MallaEncontrada->OnComponentBeginOverlap.AddDynamic(this, &AProyectilJefe::SobreSolapamientoMalla);
		}

		SetActorScale3D(FVector(3.5f, 3.5f, 3.5f));
	}

	UProjectileMovementComponent* Movimiento = Cast<UProjectileMovementComponent>(GetComponentByClass(UProjectileMovementComponent::StaticClass()));
	if (Movimiento)
	{
		Movimiento->InitialSpeed = 1200.f;
		Movimiento->MaxSpeed = 1200.f;
	}
}

// --- IMPLEMENTACI�N DE LA FUNCI�N DE DA�O ---
void AProyectilJefe::SobreSolapamientoMalla(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. Validaciones de seguridad de ingenier�a:
	// - Que el otro actor exista.
	// - Que no seamos nosotros mismos.
	// - Que sea un Pawn (jugador).
	if (OtherActor && (OtherActor != this) && OtherActor->IsA(APawn::StaticClass()))
	{
		// 2. APLICAR EL DA�O AL PIE DE LA LETRA
		// UGameplayStatics::ApplyDamage gestiona todo el sistema de da�o de Unreal.
		// DanoProyectil es la variable que definiste como 250.0f
		UGameplayStatics::ApplyDamage(OtherActor, DanoProyectil, GetInstigatorController(), this, UDamageType::StaticClass());

		// NOTA DE INGENIER�A: No llamamos a Destroy(). Queremos que el proyectil gigante atraviese al jugador
		// y siga su camino para verse m�s menacing, da�ando en el proceso.
	}
}