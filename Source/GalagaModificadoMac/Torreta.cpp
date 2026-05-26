#include "Torreta.h"
#include "Components/StaticMeshComponent.h"
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

	// INICIALIZACIÓN MANUAL (Por doble seguridad)
	JugadorObjetivo = nullptr;

	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		GetCharacterMovement()->GravityScale = 1.0f;
		GetCharacterMovement()->bConstrainToPlane = true;
	}

	// Mallas
	MallaEnemiga->SetupAttachment(GetCapsuleComponent());
	MallaEnemiga->SetSimulatePhysics(false);

	MallaCanion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCanion"));
	MallaCanion->SetupAttachment(MallaEnemiga);
	MallaCanion->SetSimulatePhysics(false);

	// Rango de Detección
	RangoDeteccion = CreateDefaultSubobject<USphereComponent>(TEXT("RangoDeteccion"));
	RangoDeteccion->SetupAttachment(RootComponent);
	RangoDeteccion->SetSphereRadius(1200.0f);

	// Esto hace que la esfera gigante sea un "fantasma" que no bloquee tus balas
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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaBase(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
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