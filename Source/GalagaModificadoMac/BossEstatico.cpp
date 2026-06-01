#include "BossEstatico.h"
#include "BossStates.h"
#include "AttackStrategies.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h" // IMPORTANTE PARA BUSCAR LAS CELDAS

ABossEstatico::ABossEstatico()
{
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaEsfera(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    MallaJefe = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaJefe"));
    RootComponent = MallaJefe;
    if (MallaEsfera.Succeeded()) { MallaJefe->SetStaticMesh(MallaEsfera.Object); }
    MallaJefe->SetWorldScale3D(FVector(15.0f, 15.0f, 15.0f));

    EscudoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EscudoMesh"));
    EscudoMesh->SetupAttachment(RootComponent);
    if (MallaEsfera.Succeeded()) { EscudoMesh->SetStaticMesh(MallaEsfera.Object); }
    EscudoMesh->SetRelativeScale3D(FVector(1.2f));

    VidaCore = 500.0f;
    CeldasActivas = 0;
    bEscudoActivo = true;

    EstadoActual = nullptr;
    EstrategiaActual = nullptr;
}

void ABossEstatico::BeginPlay()
{
    Super::BeginPlay();

    // --- EL JEFE BUSCA LAS CELDAS EN EL MAPA ---
    TArray<AActor*> CeldasEncontradas;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACeldaEnergia::StaticClass(), CeldasEncontradas);

    CeldasActivas = CeldasEncontradas.Num();

    for (AActor* ActorCelda : CeldasEncontradas)
    {
        ACeldaEnergia* Celda = Cast<ACeldaEnergia>(ActorCelda);
        if (Celda) { Celda->AsignarObservador(this); }
    }

    CambiarEstrategia(new FAtaqueParedStrategy());
    CambiarEstado(new FFase1State());
}

void ABossEstatico::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    if (EstadoActual) delete EstadoActual;
    if (EstrategiaActual) delete EstrategiaActual;
}

void ABossEstatico::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (EstadoActual) EstadoActual->EjecutarEstado(this, DeltaTime);
}

float ABossEstatico::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // Si hay celdas vivas, el jefe es inmune. El jugador DEBE ir a dispararle a las celdas.
    if (bEscudoActivo && CeldasActivas > 0) return 0.0f;

    VidaCore -= DamageAmount;
    if (VidaCore <= 0) Destroy();

    return DamageAmount;
}

void ABossEstatico::NotificarCeldaDestruida(ACeldaEnergia* CeldaQueMurio)
{
    CeldasActivas--;

    if (CeldasActivas <= 0)
    {
        bEscudoActivo = false;
        EscudoMesh->SetVisibility(false);
        EscudoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CambiarEstado(new FFase4State());
    }
}

void ABossEstatico::CambiarEstado(IBossState* NuevoEstado)
{
    if (EstadoActual) delete EstadoActual;
    EstadoActual = NuevoEstado;
}

void ABossEstatico::CambiarEstrategia(IAttackStrategy* NuevaEstrategia)
{
    if (EstrategiaActual) delete EstrategiaActual;
    EstrategiaActual = NuevaEstrategia;
}

void ABossEstatico::EjecutarAtaqueEstrategico(float DeltaTime)
{
    if (EstrategiaActual) EstrategiaActual->Ejecutar(this, DeltaTime);
}