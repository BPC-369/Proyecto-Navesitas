// Fill out your copyright notice in the Description page of Project Settings.


#include "BalaEspecial.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"

ABalaEspecial::ABalaEspecial()
{
    // mallita wiwiw
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

    if (MeshAsset.Succeeded())
    {
        if (GetProjectileMesh())
        {
            GetProjectileMesh()->SetStaticMesh(MeshAsset.Object);

            // ajustar escala de la balita
            // Esto es lo que más se va a notar visualmente
            GetProjectileMesh()->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));

            // aplicar colorsito o mallitas diferentes
            // copiar aqui mallita copiando su ruta aquí:
            static ConstructorHelpers::FObjectFinder<UMaterial> MaterialEspecial(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
            if (MaterialEspecial.Succeeded())
            {
                GetProjectileMesh()->SetMaterial(0, MaterialEspecial.Object);
            }
        }
    }

    // VELOCIDAD Y DAÑO
    if (GetProjectileMovement())
    {
        DanoProyectil = 50.0f;
        GetProjectileMovement()->InitialSpeed = 4000.f;
        GetProjectileMovement()->MaxSpeed = 4000.f;
    }
} 

void ABalaEspecial::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("¡Se ha disparado una BALA ESPECIAL!"));
}

void ABalaEspecial::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
