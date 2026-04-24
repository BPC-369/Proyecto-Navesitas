#include "NaveKamikase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ANaveKamikase::ANaveKamikase() {
    Velocidad = 700.0f;
    // Aqui falta asignarle una malla visual
}

void ANaveKamikase::Volar(float DeltaSeconds)
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

void ANaveKamikase::Explotar()
{
    // Aquí puedes agregar efectos visuales o de sonido para la explosión
    // Por ejemplo, podrías usar UGameplayStatics::SpawnEmitterAtLocation para crear una explosión visual
    // Destruye la nave después de explotar
    Destroy();
}

void ANaveKamikase::Atacar()
{
    // dejamos esta funcion vacia xq la nave kamikase no dispara
}