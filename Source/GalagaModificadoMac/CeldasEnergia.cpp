#include "CeldasEnergia.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/ConstructorHelpers.h"

ACeldaEnergia::ACeldaEnergia()
{
    PrimaryActorTick.bCanEverTick = true;

    // Raíz fija
    RaizCelda = CreateDefaultSubobject<USceneComponent>(TEXT("RaizCelda"));
    RootComponent = RaizCelda;

    // Pivote intermedio que centraremos en el offset exacto
    PivoteCentrado = CreateDefaultSubobject<USceneComponent>(TEXT("PivoteCentrado"));
    PivoteCentrado->SetupAttachment(RaizCelda);

    // Malla adjuntada al pivote
    MallaCelda = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaCelda"));
    MallaCelda->SetupAttachment(PivoteCentrado);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> NuevaMalla(TEXT("/Game/Geometry/pawn/uploads_files_4331233_UFO+4.uploads_files_4331233_UFO+4"));
    if (NuevaMalla.Succeeded())
    {
        MallaCelda->SetStaticMesh(NuevaMalla.Object);
    }

    MallaCelda->SetWorldScale3D(FVector(1.0f));
    MallaCelda->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Vida = 400.0f;
    bDestruida = false;
    MiObservador = nullptr;
}

void ACeldaEnergia::BeginPlay()
{
    Super::BeginPlay();

    // Offset exacto obtenido de GetLocalBounds (X=402.782, Y=-974.473, Z=483.138)
    FVector CentroLocal(402.782f, -974.473f, 483.138f);

    // Colocamos el pivote en ese centro relativo a la raíz
    PivoteCentrado->SetRelativeLocation(CentroLocal);

    // La malla debe estar en el origen respecto al pivote
    MallaCelda->SetRelativeLocation(FVector::ZeroVector);

    UE_LOG(LogTemp, Warning, TEXT("[%s] Pivote ajustado al centro exacto"), *GetName());
}

void ACeldaEnergia::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Rotar el pivote centrado → la malla gira sobre sí misma
    PivoteCentrado->AddLocalRotation(FRotator(0.0f, 150.0f * DeltaTime, 0.0f));
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
        {
            MiObservador->NotificarCeldaDestruida(this);
        }
        Destroy();
    }
    return DamageAmount;
}