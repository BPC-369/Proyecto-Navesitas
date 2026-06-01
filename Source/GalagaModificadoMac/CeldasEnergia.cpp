#include "CeldasEnergia.h" // <-- Debe coincidir con tu .h
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ACeldaEnergia::ACeldaEnergia()
{
    PrimaryActorTick.bCanEverTick = false;

    MallaCelda = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCelda"));
    RootComponent = MallaCelda;

    // Le asignamos forma de Cubo
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaCubo(TEXT("StaticMesh'/Game/SciFi_Props/Models/SM_Box_4.SM_Box_4'"));
    if (MallaCubo.Succeeded()) { MallaCelda->SetStaticMesh(MallaCubo.Object); }

    MallaCelda->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));

    Vida = 500.0f;
    bDestruida = false;
    MiObservador = nullptr;
}

void ACeldaEnergia::AsignarObservador(ICeldaObserver* Observador)
{
    MiObservador = Observador;
}

float ACeldaEnergia::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bDestruida) return 0.0f;

    Vida -= DamageAmount;
    if (Vida <= 0)
    {
        Vida = 0;
        bDestruida = true;
        if (MiObservador) { MiObservador->NotificarCeldaDestruida(this); }

        Destroy();
    }
    return DamageAmount;
}