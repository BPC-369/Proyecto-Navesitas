// Fill out your copyright notice in the Description page of Project Settings.


#include "EscenarioBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

AEscenarioBase::AEscenarioBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Iniciador de la Raíz
	RootEscenario = CreateDefaultSubobject<USceneComponent>(TEXT("RootEscenario"));
	RootComponent = RootEscenario;

	// crea los componentes de las paredes y el suelo
	ParedNorte = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParedNorte"));
	ParedSur = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParedSur"));
	ParedEste = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParedEste"));
	ParedOeste = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParedOeste"));
	Suelo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Suelo"));

	// Adjunta todos los componentes al Root
	ParedNorte->SetupAttachment(RootComponent);
	ParedSur->SetupAttachment(RootComponent);
	ParedEste->SetupAttachment(RootComponent);
	ParedOeste->SetupAttachment(RootComponent);
	Suelo->SetupAttachment(RootComponent);

	// dimensiones(no tocar en este apartado >:( solo en clases hijas)
	AnchoX = 10000.0f;
	LargoY = 10000.0f;
	AltoZ = 5000.0f;
	GrosorPared = 10.0f;

	ConfigurarMallaPared(ParedNorte);
	ConfigurarMallaPared(ParedSur);
	ConfigurarMallaPared(ParedEste);
	ConfigurarMallaPared(ParedOeste);

	// malla del suelo (sujeta a cambios en las clases hijas)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CuboMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialGrisAsset(TEXT("/Engine/BasicShapes/BasicAsset01.BasicAsset01"));

	if (CuboMeshAsset.Succeeded())
	{
		Suelo->SetStaticMesh(CuboMeshAsset.Object);
	}
	if (MaterialGrisAsset.Succeeded())
	{
		Suelo->SetMaterial(0, MaterialGrisAsset.Object);
	}
	//creacion del domo (para poner el skybox a las diferentes clases hijas no tocar valores aca)
	DomoCielo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DomoCielo"));
	DomoCielo->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> EsferaBasicaMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	if (EsferaBasicaMesh.Succeeded())
	{
		DomoCielo->SetStaticMesh(EsferaBasicaMesh.Object);
	}

	// escala inicial(sujeta a cmabios en clases hijas)
	DomoCielo->SetRelativeScale3D(FVector(0.0f, 0.0f, 0.0f));
	DomoCielo->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	DomoCielo->SetCollisionProfileName(TEXT("NoCollision"));
	DomoCielo->SetCastShadow(false);
}

void AEscenarioBase::ConfigurarMallaPared(UStaticMeshComponent* Pared)
{
	if (Pared)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> CuboMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
		if (CuboMesh.Succeeded())
		{
			Pared->SetStaticMesh(CuboMesh.Object);
		}

		// malla invisible para que sea un delimitador del mundo sin que se salga el jugador
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialInvisible(TEXT("Material'/Game/ParedesInvisibles.ParedesInvisibles'"));
		if (MaterialInvisible.Succeeded())
		{
			Pared->SetMaterial(0, MaterialInvisible.Object);
		}
	}
}

void AEscenarioBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEscenarioBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
// Agrégalo al final de EscenarioBase.cpp
void AEscenarioBase::GenerarObstaculosProcedurales()
{
	// Se deja vacío. Solo sirve para que los hijos hereden la firma
	// y el Linker no proteste.
}

void AEscenarioBase::AplicarEscalaFisica()
{
	FVector EscalaSuelo((AnchoX / 100.0f), (LargoY / 100.0f), 0.1f);
	Suelo->SetRelativeScale3D(EscalaSuelo);
	Suelo->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	FVector EscalaNorteSur((GrosorPared / 1000.0f), (LargoY / 1000.0f), (AltoZ / 1000.0f));
	ParedNorte->SetRelativeScale3D(EscalaNorteSur);
	ParedSur->SetRelativeScale3D(EscalaNorteSur);

	ParedNorte->SetRelativeLocation(FVector(AnchoX / 2.0f, 0.0f, (AltoZ / 2.0f)));
	ParedSur->SetRelativeLocation(FVector(-AnchoX / 2.0f, 0.0f, (AltoZ / 2.0f)));

	FVector EscalaEsteOeste(((AnchoX + (GrosorPared * 2)) / 1000.0f), (GrosorPared / 1000.0f), (AltoZ / 1000.0f));
	ParedEste->SetRelativeScale3D(EscalaEsteOeste);
	ParedOeste->SetRelativeScale3D(EscalaEsteOeste);

	ParedEste->SetRelativeLocation(FVector(0.0f, LargoY / 2.0f, (AltoZ / 2.0f)));
	ParedOeste->SetRelativeLocation(FVector(0.0f, -LargoY / 2.0f, (AltoZ / 2.0f)));
}
FVector AEscenarioBase::ObtenerPosicionSpawnSegura(float Ancho, float Largo, float Margen, float AlturaZ)
{
	FVector PosicionJugador = FVector(0.0f, 0.0f, AlturaZ); // Punto de partida de la nave
	FVector PosicionCandidata;
	float RadioSeguridad = 1500.0f; // ?? Zona de exclusión: ningún bloque a menos de 1500 unidades
	bool bPosicionValida = false;
	int32 Intentos = 0;

	// Ciclo de descarte: genera coordenadas hasta que una pase el control de distancia
	while (!bPosicionValida && Intentos < 10)
	{
		float RndX = FMath::FRandRange((-Ancho / 2.0f) + Margen, (Ancho / 2.0f) - Margen);
		float RndY = FMath::FRandRange((-Largo / 2.0f) + Margen, (Largo / 2.0f) - Margen);

		PosicionCandidata = FVector(RndX, RndY, AlturaZ);

		// Medimos la distancia euclidiana entre el candidato y el jugador
		if (FVector::Dist(PosicionCandidata, PosicionJugador) > RadioSeguridad)
		{
			bPosicionValida = true; // Superó el umbral seguro
		}
		Intentos++;
	}

	return PosicionCandidata;
}