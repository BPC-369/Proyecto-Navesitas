#include "NaveComando.h"
#include "FabricaNaves.h"
#include "GalagaModificadoMacProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "ProyectilJefe.h"
#include "ComponenteCombate.h"

ANaveComando::ANaveComando()
{
	Velocidad = 80.0f;
	FrecuenciaAtaque = 2.0f;

	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 50.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComando(TEXT("StaticMesh'/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO'"));
	if (MeshComando.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(MeshComando.Object);
		MallaEnemiga->SetRelativeScale3D(FVector(5.0f, 5.0f, 5.0f));
	}
}

void ANaveComando::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TimerAtaque, this, &ANaveComando::Atacar, FrecuenciaAtaque, true);
	GetWorld()->GetTimerManager().SetTimer(TimerSpawn, this, &ANaveComando::GestionarEscoltas, 2.5f, true);
}
//aqui evitamos el da?o aliado excluyendo la clase 
/*
float ANaveComando::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!DamageCauser) return 0.0f;

	// 1. Buscamos al causante del da?o
	AActor* Tirador = DamageCauser->GetOwner();

	// 2. Si no hay Tirador, intentamos usar al Instigator
	if (!Tirador && EventInstigator)
	{
		Tirador = EventInstigator->GetPawn();
	}

	if (Tirador)
	{
		// Si el que me dispar? es una Nave Enemiga (Cualquier tipo), da?o CERO.
		// Esto incluye a otras NaveComando y a todas las Nave_CMN (comunes).
		if (Tirador->IsA(ANaveEnemigoAereo::StaticClass()))
		{
			return 0.0f;
		}

		// Filtro de respaldo por Facci?n
		AEntidadCombate* EntidadTiradora = Cast<AEntidadCombate>(Tirador);
		if (EntidadTiradora && EntidadTiradora->Faccion == this->Faccion)
		{
			return 0.0f;
		}
	}

	// 3. Si pas? los filtros, solo puede ser el Jugador.
	VidaActual -= DamageAmount;

	if (GEngine)
	{
		FString NombreAtacante = Tirador ? Tirador->GetName() : TEXT("Desconocido");
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red,
			FString::Printf(TEXT("DA?O REAL de: %s | Vida: %.1f"), *NombreAtacante, VidaActual));
	}

	if (VidaActual <= 0) { Destroy(); }

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
*/
//la nave ataca de frente y laterales
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
//aqui determinamos donde se van a generar las naves invocadas
void ANaveComando::GestionarEscoltas()
{
	UWorld* const World = GetWorld();
	if (!World) return;

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

				// 3. Ignoramos colisiones entre la nave comando y su escolta
				NuevaEscolta->MoveIgnoreActorAdd(this);

				// 4. Agregamos a la lista
				EscoltasActivas.Add(NuevaEscolta);
			}
		}
	}
}