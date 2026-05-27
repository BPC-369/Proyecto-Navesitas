#include "Bonificacion.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GalagaModificadoMacPawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ComponenteCombate.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

int32 ABonificacion::CantidadActivas = 0;

ABonificacion::ABonificacion()
{
	PrimaryActorTick.bCanEverTick = true;

	// 1. Configuramos la esfera de colisión como la raíz
	VolumenColision = CreateDefaultSubobject<USphereComponent>(TEXT("VolumenColision"));
	RootComponent = VolumenColision;
	VolumenColision->InitSphereRadius(60.0f);
	VolumenColision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// 2. Configuramos la malla visual
	MallaBonificacion = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	MallaBonificacion->SetupAttachment(RootComponent);
	MallaBonificacion->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Solo la esfera choca

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CuboMeshAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));

	// Si Unreal encontró el modelo correctamente, se lo asignamos al componente
	if (CuboMeshAsset.Succeeded())
	{
		MallaBonificacion->SetStaticMesh(CuboMeshAsset.Object);
	}

	// 3. Valores por defecto para la animación
	VelocidadRotacion = 90.0f;
	AmplitudFlote = 20.0f;
	FrecuenciaFlote = 2.0f;
}

void ABonificacion::BeginPlay()
{
	Super::BeginPlay();

	// Guardamos la posición de spawn para el cálculo matemático del flote
	PosicionInicial = GetActorLocation();
	CantidadActivas++;
}

void ABonificacion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Animación de rotación sobre sí mismo
	AddActorLocalRotation(FRotator(0.0f, VelocidadRotacion * DeltaTime, 0.0f));

	// Animación de flote fluido usando la onda Seno
	FVector NuevaPosicion = PosicionInicial;
	NuevaPosicion.Z += FMath::Sin(GetGameTimeSinceCreation() * FrecuenciaFlote) * AmplitudFlote;
	SetActorLocation(NuevaPosicion);
}

void ABonificacion::Destroyed()
{
	// Al morir o ser recogido, restamos uno para que el Spawner pueda crear más
	CantidadActivas--;
	Super::Destroyed();
}

void ABonificacion::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AGalagaModificadoMacPawn* Jugador = Cast<AGalagaModificadoMacPawn>(OtherActor);

	if (Jugador)
	{
		// 1. Leemos si está en forma Nave o Robot usando tu método actual
		bool bEsNave = (Jugador->GetCharacterMovement()->MovementMode == MOVE_Flying);
		int32 EfectoAlAzar = FMath::RandRange(1, 4);

		// 2. Extraemos el estado que tiene el jugador ahora mismo
		IEstadoNave* EstadoAEnvolver = Jugador->EstadoActual;

		// 3. Preparamos un puntero para el nuevo súper-estado
		IEstadoNave* NuevoEstadoMejorado = nullptr;

		// 4. Elegimos la envoltura (Decorador)
		if (bEsNave)
		{
			if (EfectoAlAzar == 1) NuevoEstadoMejorado = new FDecoradorRecuperacionNave(EstadoAEnvolver, Jugador);
			else if (EfectoAlAzar == 2) NuevoEstadoMejorado = new FDecoradorCuadrupleCanon(EstadoAEnvolver, Jugador);
			else if (EfectoAlAzar == 3) NuevoEstadoMejorado = new FDecoradorBombasRacimo(EstadoAEnvolver, Jugador);
			else if (EfectoAlAzar == 4) NuevoEstadoMejorado = new FDecoradorSuperBuffoNave(EstadoAEnvolver, Jugador);
		}
		else
		{
			if (EfectoAlAzar == 1) NuevoEstadoMejorado = new FDecoradorVelocidadDash(EstadoAEnvolver, Jugador);
			else if (EfectoAlAzar == 2) NuevoEstadoMejorado = new FDecoradorCortesDistancia(EstadoAEnvolver, Jugador);
			else if (EfectoAlAzar == 3) NuevoEstadoMejorado = new FDecoradorRecuperacionRobot(EstadoAEnvolver, Jugador);
			else if (EfectoAlAzar == 4) NuevoEstadoMejorado = new FDecoradorInmunidad(EstadoAEnvolver, Jugador);
		}

		// 5. Inyectamos el nuevo comportamiento en el jugador
		if (NuevoEstadoMejorado != nullptr)
		{
			// Reemplazamos su estado actual por la versión decorada
			Jugador->EstadoActual = NuevoEstadoMejorado;
		}

		Destroy();
	}
}