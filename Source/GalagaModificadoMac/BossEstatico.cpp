#include "BossEstatico.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABossEstatico::ABossEstatico()
{
    PrimaryActorTick.bCanEverTick = true;

    CapsulaColision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsulaColision"));
    RootComponent = CapsulaColision;
    CapsulaColision->SetCapsuleSize(200.0f, 300.0f);
    CapsulaColision->SetCollisionProfileName(TEXT("Pawn"));

    MallaJefe = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaJefe"));
    MallaJefe->SetupAttachment(RootComponent);
    MallaJefe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Ruta exacta de la malla del jefe (copiada del Copy Reference)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaJefeNueva(
        TEXT("/Game/Geometry/jefeMalla/Meshy_AI_War_Machine_Chief_Ort_0603010709_texture.Meshy_AI_War_Machine_Chief_Ort_0603010709_texture")
    );
    if (MallaJefeNueva.Succeeded())
    {
        MallaJefe->SetStaticMesh(MallaJefeNueva.Object);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BossEstatico: No se encontró la malla del jefe en /Game/Geometry/jefeMalla/Meshy_AI_War_Machine_Chief_Ort_0603010709_texture"));
    }

    MallaJefe->SetWorldScale3D(FVector(15.0f, 15.0f, 15.0f));

    VidaMaxima = 200.0f;
    VidaJefe = VidaMaxima;
    CeldasActivas = 0;
    bEscudoInmune = true;
    EstrategiaActual = nullptr;

    bFuriaCeldas = false;
    bFuriaVida = false;
    IntervaloCambio = 5.0f;
    TiempoAcumuladoCambio = 0.0f;
    MultiplicadorDano = 1.0f;
    MultiplicadorVelocidad = 1.0f;
}

void ABossEstatico::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> CeldasEnMapa;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACeldaEnergia::StaticClass(), CeldasEnMapa);
    CeldasActivas = CeldasEnMapa.Num();

    for (AActor* ActorCelda : CeldasEnMapa)
    {
        ACeldaEnergia* Celda = Cast<ACeldaEnergia>(ActorCelda);
        if (Celda) { Celda->AsignarObservador(this); }
    }

    CambiarEstrategia(new FAtaqueParedStrategy());
}

void ABossEstatico::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    if (EstrategiaActual) delete EstrategiaActual;
}

void ABossEstatico::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bFuriaCeldas)
    {
        TiempoAcumuladoCambio += DeltaTime;
        if (TiempoAcumuladoCambio >= IntervaloCambio)
        {
            SeleccionarAtaqueAleatorioFuria();
            TiempoAcumuladoCambio = 0.0f;
        }
    }

    if (EstrategiaActual)
    {
        EstrategiaActual->Ejecutar(this, DeltaTime);
    }
}

float ABossEstatico::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bEscudoInmune && CeldasActivas > 0) return 0.0f;

    VidaJefe -= DamageAmount;

    if (!bFuriaVida && VidaJefe <= VidaMaxima * 0.5f)
    {
        bFuriaVida = true;
        IntervaloCambio = 2.0f;
        MultiplicadorDano = 1.5f;
        MultiplicadorVelocidad = 1.5f;
        TiempoAcumuladoCambio = 0.0f;
    }

    if (VidaJefe <= 0) Destroy();
    return DamageAmount;
}

void ABossEstatico::NotificarCeldaDestruida(ACeldaEnergia* CeldaQueMurio)
{
    CeldasActivas--;

    if (!bFuriaCeldas)
    {
        if (CeldasActivas == 2)
        {
            CambiarEstrategia(new FAtaqueOndaStrategy());
        }
        else if (CeldasActivas == 1)
        {
            CambiarEstrategia(new FAtaqueLatigoStrategy());
        }
    }

    if (CeldasActivas <= 0)
    {
        bEscudoInmune = false;

        if (!bFuriaCeldas)
        {
            bFuriaCeldas = true;
            IntervaloCambio = 5.0f;
            TiempoAcumuladoCambio = 0.0f;
            SeleccionarAtaqueAleatorioFuria();
        }
    }
}

void ABossEstatico::SeleccionarAtaqueAleatorioFuria()
{
    int32 Dado = FMath::RandRange(1, 3);
    switch (Dado)
    {
    case 1: CambiarEstrategia(new FAtaqueParedStrategy()); break;
    case 2: CambiarEstrategia(new FAtaqueOndaStrategy()); break;
    case 3: CambiarEstrategia(new FAtaqueLatigoStrategy()); break;
    }
}

void ABossEstatico::CambiarEstrategia(IAttackStrategy* NuevaEstrategia)
{
    if (EstrategiaActual) delete EstrategiaActual;
    EstrategiaActual = NuevaEstrategia;
}