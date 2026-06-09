// Fill out your copyright notice in the Description page of Project Settings.


#include "CuartelEnemigo.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

// Includes de tus 3 robots terrestres
#include "Robot_Medico.h"
#include "Robot_RZ.h"
#include "Robot_RZ_D.h"

ACuartelEnemigo::ACuartelEnemigo()
{
	PrimaryActorTick.bCanEverTick = false;

	// Componente visual básico del cuartel
	MallaEstructura = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEstructura"));
	RootComponent = MallaEstructura;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CuboMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CuboMesh.Succeeded())
	{
		MallaEstructura->SetStaticMesh(CuboMesh.Object);
		MallaEstructura->SetRelativeScale3D(FVector(5.0f, 5.0f, 8.0f)); // Lo hacemos ver como un edificio alto
	}

	// Valores por defecto seguros
	TipoRobotASpawnear = 10; // RZ por defecto
	TiempoEntreSpawns = 8.0f; // Crea un robot cada 8 segundos
}

void ACuartelEnemigo::BeginPlay()
{
	Super::BeginPlay();

	// Activamos el bucle del temporizador de producción continua
	GetWorldTimerManager().SetTimer(TimerProduccion, this, &ACuartelEnemigo::ProducirUnidad, TiempoEntreSpawns, true);
}

void ACuartelEnemigo::ProducirUnidad()
{
	UWorld* const Mundo = GetWorld();
	if (!Mundo) return;

	// El robot nacerá un poco desplazado hacia el frente del cuartel para no trabarse dentro
	FVector PosicionSpawn = GetActorLocation() + (GetActorForwardVector() * 600.0f);
	PosicionSpawn.Z = 100.0f; // Altura a ras de suelo para personajes caminantes

	FRotator RotacionSpawn = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemigoTerrestre* NuevoRobot = nullptr;


	switch (TipoRobotASpawnear)
	{
	case 9:
		NuevoRobot = Mundo->SpawnActor<ARobot_Medico>(ARobot_Medico::StaticClass(), PosicionSpawn, RotacionSpawn, SpawnParams);
		break;
	case 10:
		NuevoRobot = Mundo->SpawnActor<ARobot_RZ>(ARobot_RZ::StaticClass(), PosicionSpawn, RotacionSpawn, SpawnParams);
		break;
	case 11:
		NuevoRobot = Mundo->SpawnActor<ARobot_RZ_D>(ARobot_RZ_D::StaticClass(), PosicionSpawn, RotacionSpawn, SpawnParams);
		break;
	default:
		break;
	}

	// Opcional: Si quieres registrar el robot recién nacido en las listas del GameMode
	if (NuevoRobot)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("¡Cuartel: Unidad Terrestre Desplegada!"));
	}
}