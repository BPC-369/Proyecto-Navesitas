#include "Robot_RZ.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "TimerManager.h" 

ARobot_RZ::ARobot_RZ()
{
	PrimaryActorTick.bCanEverTick = true;

	VelocidadMovimiento = 400.0f;
	RangoAtaque = 150.0f;
	DanioBase = 20.0f;
	bEstaAtacando = false;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCono(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
    if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
    {
        MallaEnemiga->SetStaticMesh(FormaCono.Object);

        // Rotamos el cono para que la punta mire hacia el frente (Eje X)
        MallaEnemiga->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    }
}

void ARobot_RZ::Atacar()
{
    if (bEstaAtacando) return;

    bEstaAtacando = true;

    VelocidadMovimiento = VelocidadMovimiento / 2.0f;

    GetWorld()->GetTimerManager().SetTimer(TimerAtaque, this, &ARobot_RZ::EjecutarGolpeMelee, 0.5f, false);
}

void ARobot_RZ::EjecutarGolpeMelee()
{
    float DistanciaActual = CalcularDistanciaAlJugador();

    if (DistanciaActual > 0.0f && DistanciaActual <= (RangoAtaque + 20.0f))
    {
        ACharacter* Jugador = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (Jugador)
        {
            UGameplayStatics::ApplyDamage(Jugador, DanioBase, GetInstigatorController(), this, UDamageType::StaticClass());
        }
    }

    bEstaAtacando = false;
    VelocidadMovimiento = 400.0f;
}