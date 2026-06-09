#include "AttackStrategies.h"
#include "BossEstatico.h"
#include "BossAttackBuilder.h"
#include "Kismet/GameplayStatics.h"

// PATRÓN PARED
void FAtaqueParedStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 2.0f)
    {
        int32 MaxFrontales = (Jefe->CeldasActivas > 0) ? 5 : 2;

        if (ContadorAtaques < MaxFrontales)
        {
            APawn* Jugador = UGameplayStatics::GetPlayerPawn(Jefe->GetWorld(), 0);
            if (Jugador)
            {
                FVector DirJugador = (Jugador->GetActorLocation() - Jefe->GetActorLocation()).GetSafeNormal();
                BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
                float DanoBase = 200.0f, VelBase = 1800.0f;
                Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
                    .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
                    .SetEscala(2.5f);
                Builder.ConstruirMuroFrontal(Jefe->GetActorLocation(), DirJugador, 8, 250.0f, 7);
            }
            ContadorAtaques++;
        }
        else
        {
            BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
            float DanoBase = 200.0f, VelBase = 1800.0f;
            Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
                .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
                .SetEscala(2.2f);

            static float AnguloGlobal = 0.0f;
            Builder.ConstruirMuroEspiral(Jefe->GetActorLocation(), 12, 6, 200.0f, 4, AnguloGlobal);
            AnguloGlobal += 30.0f;

            ContadorAtaques = 0;
        }
        TiempoAcumulado = 0.0f;
    }
}

// PATRÓN ONDA
void FAtaqueOndaStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 1.6f)
    {
        int32 MaxFrontales = (Jefe->CeldasActivas > 0) ? 5 : 2;

        if (ContadorAtaques < MaxFrontales)
        {
            APawn* Jugador = UGameplayStatics::GetPlayerPawn(Jefe->GetWorld(), 0);
            if (Jugador)
            {
                FVector DirJugador = (Jugador->GetActorLocation() - Jefe->GetActorLocation()).GetSafeNormal();
                BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
                float DanoBase = 300.0f, VelBase = 2000.0f;
                Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
                    .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
                    .SetEscala(2.0f);
                Builder.ConstruirMuroFrontal(Jefe->GetActorLocation(), DirJugador, 6, 200.0f, 5);
            }
            ContadorAtaques++;
        }
        else
        {
            BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
            float DanoBase = 300.0f, VelBase = 2000.0f;
            Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
                .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
                .SetEscala(2.0f);
            Builder.ConstruirMuroOndaExpansiva(Jefe->GetActorLocation(), 36, 6, 180.0f, 2);
            ContadorAtaques = 0;
        }
        TiempoAcumulado = 0.0f;
    }
}

// PATRÓN LÁTIGO
void FAtaqueLatigoStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 0.25f)
    {
        int32 MaxFrontales = (Jefe->CeldasActivas > 0) ? 5 : 2;

        if (ContadorAtaques < MaxFrontales)
        {
            APawn* Jugador = UGameplayStatics::GetPlayerPawn(Jefe->GetWorld(), 0);
            if (Jugador)
            {
                FVector DirJugador = (Jugador->GetActorLocation() - Jefe->GetActorLocation()).GetSafeNormal();
                BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
                float DanoBase = 400.0f, VelBase = 2400.0f;
                Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
                    .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
                    .SetEscala(1.5f);
                Builder.ConstruirMuroFrontal(Jefe->GetActorLocation(), DirJugador, 4, 180.0f, 4);
            }
            ContadorAtaques++;
        }
        else
        {
            AnguloGiro += 25.0f;
            if (AnguloGiro >= 360.0f) AnguloGiro -= 360.0f;

            BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
            float DanoBase = 400.0f, VelBase = 2400.0f;
            Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
                .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
                .SetEscala(1.5f);
            Builder.ConstruirMuroEspiral(Jefe->GetActorLocation(), 8, 3, 180.0f, 2, AnguloGiro);
            ContadorAtaques = 0;
        }
        TiempoAcumulado = 0.0f;
    }
}