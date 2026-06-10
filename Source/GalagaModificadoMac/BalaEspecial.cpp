#include "BalaEspecial.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ABalaEspecial::ABalaEspecial()
{
	// Malla
	if (GetProjectileMesh())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));
		if (MeshAsset.Succeeded())
		{
			GetProjectileMesh()->SetStaticMesh(MeshAsset.Object);
			GetProjectileMesh()->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));

			static ConstructorHelpers::FObjectFinder<UMaterial> MaterialEspecial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
			if (MaterialEspecial.Succeeded())
				GetProjectileMesh()->SetMaterial(0, MaterialEspecial.Object);
		}
	}

	// Movimiento
	if (GetProjectileMovement())
	{
		GetProjectileMovement()->InitialSpeed = 4000.f;
		GetProjectileMovement()->MaxSpeed = 4000.f;
	}

	DanoProyectil = 50.0f;
}