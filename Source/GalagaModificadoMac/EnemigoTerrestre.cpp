#include "EnemigoTerrestre.h"
#include "Kismet/GameplayStatics.h" // Necesario para buscar al jugador
#include "GameFramework/Character.h" // Necesario para referencias del jugador

AEnemigoTerrestre::AEnemigoTerrestre()
{
    PrimaryActorTick.bCanEverTick = true;

    VelocidadMovimiento = 300.0f;
    RangoDeteccion = 500.0f;
    DanioBase = 10.0f;
    RangoAtaque = 150.0f; 

    MallaEnemiga = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEnemiga"));
    MallaEnemiga->SetupAttachment(RootComponent);
    // 1. Obligamos al enemigo a generar el evento de impacto
    MallaEnemiga->SetNotifyRigidBodyCollision(true);
    // 2. Lo convertimos en un muro sólido que bloquee todo
    //MallaEnemiga->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void AEnemigoTerrestre::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    ActualizarComportamiento();
}

void AEnemigoTerrestre::Mover()
{
    if (VelocidadMovimiento <= 0.0f) return;

    APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Jugador)
    {
        FVector MiPosicion = GetActorLocation();
        FVector PosicionJugador = Jugador->GetActorLocation();

        // 1. Calculamos la dirección pura
        FVector DireccionHaciaJugador = PosicionJugador - MiPosicion;

        // 2. Anulamos el eje Z ANTES de normalizar (Tu intuición aplicada)
        DireccionHaciaJugador.Z = 0.0f;

        // 3. Ahora normalizamos para que la fuerza total sea 1.0 en X e Y
        DireccionHaciaJugador.Normalize();

        FVector NuevoMovimiento = DireccionHaciaJugador * VelocidadMovimiento * GetWorld()->GetDeltaSeconds();

        // 4. Cambiamos el Sweep a 'false'. 
        // Ahora el robot se deslizará ignorando la fricción del piso que lo bloqueaba.
        AddActorWorldOffset(NuevoMovimiento, false);
    }
}

float AEnemigoTerrestre::CalcularDistanciaAlJugador()
{
    APawn* Jugador = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Jugador)
    {
        return FVector::Distance(GetActorLocation(), Jugador->GetActorLocation());
    }
    return -1.0f;
}

bool AEnemigoTerrestre::DetectarObjetivo()
{
    float Distancia = CalcularDistanciaAlJugador();
    return (Distancia > 0.0f && Distancia <= RangoDeteccion);
}

void AEnemigoTerrestre::ActualizarComportamiento()
{
	bool bJugadorEnLaMira = DetectarObjetivo();

	if (bJugadorEnLaMira)
	{
		Mover(); 

		float DistanciaActual = CalcularDistanciaAlJugador();
		if (DistanciaActual <= RangoAtaque) 
		{
			Atacar();
		}
	}
}

void AEnemigoTerrestre::Atacar()
{
}