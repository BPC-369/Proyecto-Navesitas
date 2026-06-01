#include "AttackStrategies.h"
#include "BossEstatico.h"
#include "BossAttackBuilder.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

void FAtaqueParedStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 3.0f) // Dispara cada 3 segundos
    {
        BossAttackBuilder ConstructorAtaque(Jefe->GetWorld(), Jefe);
        ConstructorAtaque.SetDano(30.0f).SetVelocidad(800.0f).SetEscala(2.0f);

        // --- ATAQUE EN 4 DIRECCIONES ---
        FVector Direcciones[4] = {
            FVector(1, 0, 0),  // Adelante
            FVector(-1, 0, 0), // Atrás
            FVector(0, 1, 0),  // Derecha
            FVector(0, -1, 0)  // Izquierda
        };

        for (int i = 0; i < 4; ++i)
        {
            // Alejamos el origen para que no choque con el cuerpo gigante del jefe
            FVector Origen = Jefe->GetActorLocation() + (Direcciones[i] * 900.0f);
            ConstructorAtaque.ConstruirPared(Origen, Direcciones[i], 2, 5, 250.0f);
        }

        TiempoAcumulado = 0.0f;
    }
}

void FAtaqueOndaStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 2.0f)
    {
        // --- BUSCAMOS AL JUGADOR PARA ATACARLO DIRECTO ---
        FVector DirAtaque = Jefe->GetActorForwardVector();
        APawn* Jugador = UGameplayStatics::GetPlayerPawn(Jefe->GetWorld(), 0);

        if (Jugador)
        {
            DirAtaque = (Jugador->GetActorLocation() - Jefe->GetActorLocation()).GetSafeNormal();
            DirAtaque.Z = 0; // Ignoramos la altura para que no dispare al suelo ni al cielo
            DirAtaque.Normalize();
        }

        BossAttackBuilder ConstructorAtaque(Jefe->GetWorld(), Jefe);
        ConstructorAtaque.SetDano(20.0f).SetVelocidad(1200.0f).SetEscala(1.5f);

        FVector Origen = Jefe->GetActorLocation() + (DirAtaque * 900.0f);
        ConstructorAtaque.ConstruirAbanico(Origen, DirAtaque, 7, 75.0f, false);

        TiempoAcumulado = 0.0f;
    }
}

void FAtaqueLatigoStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 0.1f)
    {
        Angulo += 15.0f;
        if (Angulo >= 360.0f) Angulo -= 360.0f;

        BossAttackBuilder ConstructorAtaque(Jefe->GetWorld(), Jefe);
        ConstructorAtaque.SetDano(10.0f).SetVelocidad(1500.0f).SetEscala(1.0f);

        FVector Origen = Jefe->GetActorLocation(); // Dispara desde el centro girando

        // 4 rayos giratorios constantes
        for (int i = 0; i < 4; ++i)
        {
            FVector Direccion = FVector(1, 0, 0).RotateAngleAxis(Angulo + (i * 90.0f), FVector::UpVector);
            FVector SpawnPos = Origen + (Direccion * 900.0f);
            ConstructorAtaque.ConstruirPared(SpawnPos, Direccion, 1, 1, 0.0f);
        }
        TiempoAcumulado = 0.0f;
    }
}