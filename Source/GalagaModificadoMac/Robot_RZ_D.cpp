// Fill out your copyright notice in the Description page of Project Settings.


#include "Robot_RZ_D.h"
#include "TimerManager.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GalagaModificadoMacProjectile.h"
#include "ComponenteCombate.h"

ARobot_RZ_D::ARobot_RZ_D()
{
    VelocidadMovimiento = 150.0f;
    RangoAtaque = 1000.0f;
    DanioBase = 10.0f;
    bArmaCargada = true;

    // Dispara a los 10 metros, pero empieza a esquivar a los 6 metros
    DistanciaOptimaDeCombate = 600.0f;
    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 50.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->EscudoMaximo = 0.0f;
        ComponenteCombate->EscudoActual = ComponenteCombate->EscudoMaximo;
        ComponenteCombate->Faccion = FName("Enemigo");
    }
}

// Mantenemos exactamente el mismo Mover() con el Producto Cruz de la respuesta anterior
void ARobot_RZ_D::Mover()
{
    float DistanciaActual = CalcularDistanciaAlJugador();

    if (DistanciaActual > DistanciaOptimaDeCombate)
    {
        // Si está lejos, camina recto usando la fórmula del abuelo
        Super::Mover();
    }
    else
    {
        // Si llegó a los 6 metros, hace el movimiento lateral de oscilación (-1 a 1)
        ACharacter* Jugador = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (Jugador)
        {
            FVector MiPosicion = GetActorLocation();
            FVector PosicionJugador = Jugador->GetActorLocation();

            FVector DireccionAdelante = (PosicionJugador - MiPosicion).GetSafeNormal();
            DireccionAdelante.Z = 0.0f;

            FVector DireccionLateral = FVector::CrossProduct(FVector::UpVector, DireccionAdelante).GetSafeNormal();
            float Oscilacion = FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f);

            FVector NuevoMovimiento = DireccionLateral * VelocidadMovimiento * Oscilacion * GetWorld()->GetDeltaSeconds();
            AddActorWorldOffset(NuevoMovimiento, true);
        }
    }
}

// Su ataque sigue siendo simple y limpio
void ARobot_RZ_D::Atacar()
{
    UWorld* const World = GetWorld();
    if (World != nullptr)
    {
        const FRotator RotacionDisparo = GetActorRotation();

        const FVector PosicionDisparo = GetActorLocation() + (GetActorForwardVector() * 100.0f);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparo, RotacionDisparo, SpawnParams);
    }
}