#include "CeldasEnergia.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"

ACeldaEnergia::ACeldaEnergia()
{
    PrimaryActorTick.bCanEverTick = true;

    RaizCelda = CreateDefaultSubobject<USceneComponent>(TEXT("RaizCelda"));
    RootComponent = RaizCelda;

    PivoteCentrado = CreateDefaultSubobject<USceneComponent>(TEXT("PivoteCentrado"));
    PivoteCentrado->SetupAttachment(RaizCelda);

    MallaCelda = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCelda"));
    MallaCelda->SetupAttachment(PivoteCentrado);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> NuevaMalla(TEXT("/Game/Geometry/pawn/uploads_files_4331233_UFO+4.uploads_files_4331233_UFO+4"));
    if (NuevaMalla.Succeeded())
        MallaCelda->SetStaticMesh(NuevaMalla.Object);

    MallaCelda->SetWorldScale3D(FVector(1.0f));
    MallaCelda->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Vida = 400.0f;
    bDestruida = false;
    MiObservador = nullptr;
}

void ACeldaEnergia::BeginPlay()
{
    Super::BeginPlay();

    FVector CentroLocal(402.782f, -974.473f, 483.138f);
    PivoteCentrado->SetRelativeLocation(CentroLocal);
    MallaCelda->SetRelativeLocation(FVector::ZeroVector);
}

void ACeldaEnergia::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    PivoteCentrado->AddLocalRotation(FRotator(0.0f, 10.0f * DeltaTime, 0.0f));
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
        if (MiObservador)
            MiObservador->NotificarCeldaDestruida(this);
        Destroy();
    }
    return DamageAmount;
}