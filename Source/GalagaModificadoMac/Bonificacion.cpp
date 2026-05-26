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

	// Verificamos si quien nos tocó fue el jugador
	AGalagaModificadoMacPawn* Jugador = Cast<AGalagaModificadoMacPawn>(OtherActor);

	if (Jugador)
	{
		// Descubrimos la forma actual leyendo el motor de físicas
		bool bEsNave = (Jugador->GetCharacterMovement()->MovementMode == MOVE_Flying);
		int32 EfectoAlAzar = FMath::RandRange(1, 4);

		if (bEsNave)
		{
			switch (EfectoAlAzar)
			{
			case 1: // Recuperación Total
				if (Jugador->ComponenteCombate) {
					Jugador->ComponenteCombate->VidaActual = Jugador->ComponenteCombate->VidaMaxima;
					Jugador->ComponenteCombate->EscudoActual = Jugador->ComponenteCombate->EscudoMaximo;
				}
				break;
			case 2: // Cuádruple Cañón
				Jugador->TiempoDisparoCuadruple = 10.0f; // Dura 10 segundos
				break;
			case 3: // Bombas Racimo
				Jugador->BombasRacimoRestantes += 6; // Suma 6 disparos especiales
				break;
			case 4: // Super Buffo Nave
				Jugador->VelocidadOriginalNave = Jugador->MoveSpeed;
				Jugador->MoveSpeed *= 2.0f;
				Jugador->GetCharacterMovement()->MaxFlySpeed = Jugador->MoveSpeed;
				Jugador->MultiplicadorDanio = 1.5f;
				Jugador->TiempoBuffoNave = 8.0f;
				break;
			}
		}
		else
		{
			switch (EfectoAlAzar)
			{
			case 1: // Buffo Velocidad y Dash
				Jugador->MoveSpeed = 800.0f; // Velocidad aumentada
				Jugador->GetCharacterMovement()->MaxWalkSpeed = Jugador->MoveSpeed;
				Jugador->TiempoBuffoRobot = 10.0f;
				break;
			case 2: // Cortes a distancia
				Jugador->TiempoCortesDistancia = 12.0f;
				break;
			case 3: // Recuperación Total Robot
				if (Jugador->ComponenteCombate) {
					Jugador->ComponenteCombate->VidaActual = Jugador->ComponenteCombate->VidaMaxima;
					Jugador->ComponenteCombate->EscudoActual = Jugador->ComponenteCombate->EscudoMaximo;
				}
				break;
			case 4: // Inmunidad + Doble Daño
				Jugador->TiempoInmunidad = 8.0f;
				Jugador->MultiplicadorDanio = 2.0f;
				break;
			}
		}

		// Opcional: Reproducir sonido de recolección
		// if (SonidoRecoleccion) UGameplayStatics::PlaySoundAtLocation(this, SonidoRecoleccion, GetActorLocation());

		// Nos destruimos al ser recogidos
		Destroy();
	}
}