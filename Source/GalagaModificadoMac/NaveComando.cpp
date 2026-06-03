#include "NaveComando.h"
#include "FabricaNaves.h"
#include "GalagaModificadoMacProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "ProyectilJefe.h"
#include "ComponenteCombate.h"
#include "Components/PrimitiveComponent.h"

ANaveComando::ANaveComando()
{
	Velocidad = 10.0f;
	FrecuenciaAtaque = 2.0f;

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 2500.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComando(TEXT("StaticMesh'/Game/Geometry/sasa/StarSparrow05.StarSparrow05'"));
	if (MeshComando.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(MeshComando.Object);
		MallaEnemiga->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
		MallaEnemiga->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void ANaveComando::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerAtaque, this, &ANaveComando::Atacar, FrecuenciaAtaque, true);
	GetWorld()->GetTimerManager().SetTimer(TimerSpawn, this, &ANaveComando::GestionarEscoltas, 2.5f, true);
}

// La nave ataca de frente y laterales
void ANaveComando::Atacar()
{
	UWorld* const World = GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Frente = GetActorLocation() + (GetActorForwardVector() * 400.0f);
	World->SpawnActor<AProyectilJefe>(Frente, GetActorRotation(), Params);

	FVector Derecha = GetActorLocation() + (GetActorRightVector() * 450.0f);
	FVector Izquierda = GetActorLocation() - (GetActorRightVector() * 450.0f);

	World->SpawnActor<AProyectilJefe>(Derecha, GetActorRightVector().Rotation(), Params);
	World->SpawnActor<AProyectilJefe>(Izquierda, (-GetActorRightVector()).Rotation(), Params);
}

// Aquí determinamos dónde se van a generar las naves invocadas
void ANaveComando::GestionarEscoltas()
{
	UWorld* const World = GetWorld();
	if (!World) return;

	// Limpieza de escoltas muertas
	for (int32 i = EscoltasActivas.Num() - 1; i >= 0; i--)
	{
		if (!IsValid(EscoltasActivas[i])) { EscoltasActivas.RemoveAt(i); }
	}

	if (EscoltasActivas.Num() <= (MAX_ESCOLTAS - NAVES_POR_SPAWN))
	{
		for (int32 j = 0; j < NAVES_POR_SPAWN; j++)
		{
			FVector Offset = (GetActorRightVector() * ((j - 1) * 450.0f)) - (GetActorForwardVector() * 600.0f);
			FVector Pos = GetActorLocation() + Offset;

			FabricaNaves::TipoNave TipoASpawnear = FMath::RandBool() ? FabricaNaves::COMUN : FabricaNaves::KAMIKASE;
			ANaveEnemigoAereo* NuevaEscolta = FabricaNaves::CrearNave(TipoASpawnear, World, Pos, GetActorRotation());
			if (NuevaEscolta)
			{
				// 1. Buscamos el chip de la nave recién creada
				UComponenteCombate* CompEscolta = NuevaEscolta->FindComponentByClass<UComponenteCombate>();

				// 2. Le pasamos nuestra facción de forma segura
				if (CompEscolta && ComponenteCombate)
				{
					CompEscolta->Faccion = ComponenteCombate->Faccion;
				}

				// 3. Ignoramos colisiones entre la nave comando y su escolta (TU MÉTODO)
				UPrimitiveComponent* ColliderEscolta = NuevaEscolta->FindComponentByClass<UPrimitiveComponent>();
				if (ColliderEscolta)
				{
					ColliderEscolta->IgnoreActorWhenMoving(this, true);
				}

				// 4. Agregamos a la lista
				EscoltasActivas.Add(NuevaEscolta);
			}
		}
	}
}