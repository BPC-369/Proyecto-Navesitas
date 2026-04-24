// Copyright Epic Games, Inc. All Rights Reserve

#include "GalagaModificadoMacProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

AGalagaModificadoMacProjectile::AGalagaModificadoMacProjectile() 
{
	InitialLifeSpan = 3.0f; // despues de 3 segs la bala se destruye si no ha chocado con nada
	DanoProyectil = 25.0f; // El daño que tendra la bala 
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object); // aplicamos la malla a la bala
	ProjectileMesh->SetupAttachment(RootComponent); 
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile"); // hacemos que la bala pueda chocar con todo menos el que la dispara
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AGalagaModificadoMacProjectile::OnHit); // cada vez que la bala choque con algo se llamara a la funcion OnHit para aplicar dano y fisicas
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));// Creamos el componente de movimiento para la bala
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;// La velocidad inicial de la bala
	ProjectileMovement->MaxSpeed = 3000.f;// La velocidad Maxima de la bala
	ProjectileMovement->bRotationFollowsVelocity = true; // La bala se orienta hacia la dirección a la que va
	ProjectileMovement->bShouldBounce = false; // La bala no rebota al chocar con algo
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity
}

void AGalagaModificadoMacProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// si el actor con el que chocamos existe, no es esta bala y no nos chocamos con nosotros mismos, entonces aplicamos dano y fisicas
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		// 2. EL SISTEMA NATIVO: Ahora sí, le aplicamos daño a todo lo que la bala toque.
		// Tu EntidadCombate recibirá este llamado y ejecutará tu lógica perfecta.
		UGameplayStatics::ApplyDamage(OtherActor, DanoProyectil, nullptr, this, UDamageType::StaticClass());

		// 3. Físicas para objetos inertes: El empujón se queda aislado aquí adentro.
		if (OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		}
	}

	// zLa bala se destruye
	Destroy();
}