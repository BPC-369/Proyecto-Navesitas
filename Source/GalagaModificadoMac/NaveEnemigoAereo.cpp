// Fill out your copyright notice in the Description page of Project Settings.


#include "NaveEnemigoAereo.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EntidadCombate.h"

ANaveEnemigoAereo::ANaveEnemigoAereo()
{
    PrimaryActorTick.bCanEverTick = true;

    Velocidad = 500.0f;        // 
    DanioAtaque = 1000.0f;       // 
    FrecuenciaAtaque = 2.0f;   // 
    bTieneEscudo = false;

    MallaEnemiga = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEnemiga"));
    RootComponent = MallaEnemiga;
    // 1. Obligamos al enemigo a generar el evento de impacto
    MallaEnemiga->SetNotifyRigidBodyCollision(true);
    // 2. Lo convertimos en un muro sólido que bloquee todo
    MallaEnemiga->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}
/*
void ANaveEnemigoAereo::BeginPlay()
{
    Super::BeginPlay();
    MallaEnemiga->OnComponentBeginOverlap.AddDynamic(this, &ANaveEnemigoAereo::AlChocar);
}
*/
void ANaveEnemigoAereo::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Todas las naves vuelan en cada frame
    Volar(DeltaSeconds);
}

void ANaveEnemigoAereo::Volar(float DeltaSeconds)
{
    // Buscamos al jugador para tener una referencia de hacia dónde ir
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (PlayerPawn) // si el jugador existe
    {
        // Calculamos la dirección hacia el jugador y la normalizamos para obtener un vector unitario
        FVector DireccionHaciaJugador = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();

        // Aplicamos el movimiento tridimensional (Arriba, Abajo, Izquierda, Derecha)
        FVector NuevoDesplazamiento = DireccionHaciaJugador * Velocidad * DeltaSeconds;

        // Movemos a la nave físicamente en el espacio
        AddActorWorldOffset(NuevoDesplazamiento, true);

        // Hacemos que la nave "mire" hacia donde vuela
        FRotator NuevaRotacion = DireccionHaciaJugador.Rotation();
        SetActorRotation(NuevaRotacion);
    }
}

void ANaveEnemigoAereo::Atacar()
{
    // Esta función se queda vacía aquí. 
    // La Nave Común disparará, pero la Kamikaze usará esta función para explotar.
}
/*
void ANaveEnemigoAereo::AlChocar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // TRAMPA 1: Tiene que estar en la LÍNEA 1, antes de cualquier "if"
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("¡ALGO ME ATRAVESÓ!"));

    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        // TRAMPA 2: Verificamos quién nos atravesó
        FString NombreActor = OtherActor->GetName();
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, FString::Printf(TEXT("Atravesé a: %s"), *NombreActor));

        // Intentamos identificar si es una Entidad de Combate
        AEntidadCombate* EntidadGolpeada = Cast<AEntidadCombate>(OtherActor);

        if (EntidadGolpeada != nullptr)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, TEXT("¡SÍ ES UNA ENTIDAD DE COMBATE!"));

            if (EntidadGolpeada->Faccion == FName("Jugador"))
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("¡APLICANDO DAÑO AL JUGADOR!"));
                float DanoA_Aplicar = DanioAtaque / 2.0f;
                UGameplayStatics::ApplyDamage(OtherActor, DanoA_Aplicar, nullptr, this, UDamageType::StaticClass());
            }
        }
        else
        {
            // TRAMPA 3: El Cast falló
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("ERROR: Lo que toqué NO hereda de EntidadCombate."));
        }
    }
}
*/
