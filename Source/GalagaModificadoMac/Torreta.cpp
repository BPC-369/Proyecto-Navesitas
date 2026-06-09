#include "Torreta.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h" 
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GalagaModificadoMacProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "ComponenteCombate.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Components/CapsuleComponent.h"

ATorreta::ATorreta()
{
	PrimaryActorTick.bCanEverTick = true;

	JugadorObjetivo = nullptr;

	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		GetCharacterMovement()->GravityScale = 1.0f;
		GetCharacterMovement()->bConstrainToPlane = true;
	}


	if (MallaEnemiga)
	{
		MallaEnemiga->SetVisibility(false);
		MallaEnemiga->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


	MeshTorretaAnimada = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshTorretaAnimada"));
	if (MeshTorretaAnimada)
	{
		MeshTorretaAnimada->SetupAttachment(GetCapsuleComponent());
		MeshTorretaAnimada->SetSimulatePhysics(false);

		MeshTorretaAnimada->SetRelativeLocation(FVector(0.0f, 0.0f, -140.0f));
		// Pega tu ruta exacta aquí adentro si cambia de nombre:
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> MallaTorretaAsset(TEXT("SkeletalMesh'/Game/Modelos/ciudaddestruida/torreta/TorretaAnimada.TorretaAnimada'"));
		if (MallaTorretaAsset.Succeeded())
		{
			MeshTorretaAnimada->SetSkeletalMesh(MallaTorretaAsset.Object);
		}
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialTorretaAsset(TEXT("Material'/Game/Modelos/ciudaddestruida/torreta/mtorreta.mtorreta'"));

		if (MaterialTorretaAsset.Succeeded())
		{
			// Le inyectamos el material al Slot 0 de la malla animada
			MeshTorretaAnimada->SetMaterial(0, MaterialTorretaAsset.Object);
		}
	}

	// Mallas (El cañón se acopla a la nueva malla animada para heredar su posición de disparo)
	MallaCanion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCanion"));
	MallaCanion->SetupAttachment(MeshTorretaAnimada ? Cast<USceneComponent>(MeshTorretaAnimada) : GetCapsuleComponent());
	MallaCanion->SetSimulatePhysics(false);

	// Rango de Detección
	RangoDeteccion = CreateDefaultSubobject<USphereComponent>(TEXT("RangoDeteccion"));
	RangoDeteccion->SetupAttachment(RootComponent);
	RangoDeteccion->SetSphereRadius(1200.0f);

	RangoDeteccion->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RangoDeteccion->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RangoDeteccion->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	CadenciaAtaque = 1.0f;
	DistanciaParaAcelerar = 600.0f;

	// Componente de Vida
	if (ComponenteCombate != nullptr)
	{
		ComponenteCombate->VidaMaxima = 150.0f;
		ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
		ComponenteCombate->Faccion = FName("Enemigo");
	}

	// Dejamos esto quieto por si la clase base necesita procesar algo, pero ya no afectará visualmente
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaBase(TEXT(""));
	if (FormaBase.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(FormaBase.Object);
	}
}



void ATorreta::BeginPlay()
{
	Super::BeginPlay();
	JugadorObjetivo = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	GetWorldTimerManager().SetTimer(TimerDisparo, this, &ATorreta::Atacar, CadenciaAtaque, true);
}

void ATorreta::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (JugadorObjetivo && RangoDeteccion->IsOverlappingActor(JugadorObjetivo))
	{
		FVector Direccion = JugadorObjetivo->GetActorLocation() - MallaCanion->GetComponentLocation();
		FRotator NuevaRotacion = Direccion.Rotation();

		MallaCanion->SetWorldRotation(FMath::RInterpTo(MallaCanion->GetComponentRotation(), NuevaRotacion, DeltaTime, 5.0f));

		float DistanciaActual = FVector::Dist(GetActorLocation(), JugadorObjetivo->GetActorLocation());
		float NuevaCadencia = (DistanciaActual < DistanciaParaAcelerar) ? 0.5f : CadenciaAtaque;

		if (!GetWorldTimerManager().IsTimerActive(TimerDisparo) || GetWorldTimerManager().GetTimerRate(TimerDisparo) != NuevaCadencia)
		{
			GetWorldTimerManager().SetTimer(TimerDisparo, this, &ATorreta::Atacar, NuevaCadencia, true);
		}
	}
}

void ATorreta::Atacar()
{
	if (JugadorObjetivo && RangoDeteccion->IsOverlappingActor(JugadorObjetivo))
	{
		FVector SpawnLoc = MallaCanion->GetComponentLocation() + (MallaCanion->GetForwardVector() * 500.0f);
		FRotator SpawnRot = MallaCanion->GetComponentRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (GetWorld())
		{
			AActor* Proyectil = GetWorld()->SpawnActor<AGalagaModificadoMacProjectile>(AGalagaModificadoMacProjectile::StaticClass(), SpawnLoc, SpawnRot, SpawnParams);

			if (Proyectil)
			{
				Proyectil->SetActorScale3D(FVector(3.0f, 3.0f, 3.0f));

				UPrimitiveComponent* ColisionBala = Cast<UPrimitiveComponent>(Proyectil->GetRootComponent());
				if (ColisionBala)
				{
					ColisionBala->IgnoreActorWhenMoving(this, true);
				}
			}
		}
	}
}