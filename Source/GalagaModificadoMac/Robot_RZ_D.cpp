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
    RangoDeteccion = 1500.0f;
    RangoAtaque = 1000.0f;
    DanioBase = 10.0f;
    bPuedeDisparar = true;
    DistanciaOptimaDeCombate = 600.0f;

    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 50.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->EscudoMaximo = 0.0f;
        ComponenteCombate->EscudoActual = ComponenteCombate->EscudoMaximo;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FormaCono(TEXT("SkeletalMesh'/Game/Geometry/RazoDistancia/mallaRobotLider.mallaRobotLider'"));

    if (FormaCono.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(FormaCono.Object);
        GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

        GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
        GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimacionRobotAsset(TEXT("AnimBlueprint'/Game/Blueprints/ABP_RZ_D.ABP_RZ_D_C'"));

    if (AnimacionRobotAsset.Succeeded())
    {
        GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint); // Añade esta línea por seguridad
        GetMesh()->SetAnimInstanceClass(AnimacionRobotAsset.Class);
    }
}

// Mantenemos exactamente el mismo Mover() con el Producto Cruz de la respuesta anterior
void ARobot_RZ_D::Mover()
{
    ACharacter* Jugador = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Jugador) return;

    FVector MiPosicion = GetActorLocation();
    FVector PosicionJugador = Jugador->GetActorLocation();
    float DistanciaActual = FVector::Dist(MiPosicion, PosicionJugador);

    // Vector que apunta directo hacia ti
    FVector DireccionHaciaJugador = (PosicionJugador - MiPosicion).GetSafeNormal();
    DireccionHaciaJugador.Z = 0.0f;

    // 1. EL CUELLO: Siempre te mira de frente, sin importar hacia dónde camine
    SetActorRotation(DireccionHaciaJugador.Rotation());

    // 2. LAS 3 ZONAS TÁCTICAS

    if (DistanciaActual > RangoAtaque)
    {
        // ZONA 1 (MUY LEJOS): Fuera de rango de disparo. 
        // Camina hacia adelante para entrar en combate.
        AddMovementInput(DireccionHaciaJugador, 1.0f);
    }
    else if (DistanciaActual >= 500.0f && DistanciaActual <= RangoAtaque)
    {
        // ZONA 2 (DISTANCIA ÓPTIMA): Está a la distancia perfecta para disparar.
        // Solo hace Zigzag lateral para esquivar tus ataques.
        FVector DireccionLateral = FVector::CrossProduct(FVector::UpVector, DireccionHaciaJugador).GetSafeNormal();
        float Oscilacion = FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f);

        AddMovementInput(DireccionLateral * Oscilacion, 1.0f);
    }
    else
    {
        // ZONA 3 (MUY CERCA): Estás a menos de 5 metros (500 unidades).
        // KITING: Invertimos el vector para que camine en reversa alejándose de ti.
        FVector DireccionEscape = -DireccionHaciaJugador;
        AddMovementInput(DireccionEscape, 1.0f);
    }
}

// Su ataque sigue siendo simple y limpio
void ARobot_RZ_D::Atacar()
{
    if (!bPuedeDisparar) return;

    bPuedeDisparar = false;

    UWorld* const World = GetWorld();
    ACharacter* Jugador = UGameplayStatics::GetPlayerCharacter(World, 0); // Necesitamos al jugador

    if (World != nullptr && Jugador != nullptr)
    {
        // 1. Posición de salida de la bala
        const FVector PosicionDisparo = GetActorLocation() + (GetActorForwardVector() * 100.0f);

        // 2. MATEMÁTICA PURA: Calculamos el ángulo exacto hacia tu nave azul
        FVector DireccionAlJugador = Jugador->GetActorLocation() - PosicionDisparo;
        const FRotator RotacionDisparo = DireccionAlJugador.Rotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        World->SpawnActor<AGalagaModificadoMacProjectile>(PosicionDisparo, RotacionDisparo, SpawnParams);

        if (AnimacionDisparo != nullptr)
        {
            PlayAnimMontage(AnimacionDisparo);
        }

        World->GetTimerManager().SetTimer(TimerDisparo, this, &ARobot_RZ_D::ResetearDisparo, 1.5f, false);
    }
}

void ARobot_RZ_D::ResetearDisparo()
{
    bPuedeDisparar = true;
}