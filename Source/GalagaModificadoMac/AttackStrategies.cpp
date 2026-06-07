#include "AttackStrategies.h"
#include "BossEstatico.h"
#include "BossAttackBuilder.h"

void FAtaqueParedStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 2.0f)
    {
        BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
        float DanoBase = 200.0f, VelBase = 1800.0f;
        Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
            .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
            .SetEscala(2.2f);
        Builder.ConstruirMuroRadialAoE(Jefe->GetActorLocation(), 8, 3, 250.0f, 5);
        TiempoAcumulado = 0.0f;
    }
}

void FAtaqueOndaStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 1.6f)
    {
        BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
        float DanoBase = 300.0f, VelBase = 2000.0f;
        Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
            .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
            .SetEscala(2.0f);
        Builder.ConstruirMuroRadialAoE(Jefe->GetActorLocation(), 16, 4, 200.0f, 5);
        TiempoAcumulado = 0.0f;
    }
}

void FAtaqueLatigoStrategy::Ejecutar(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoAcumulado += DeltaTime;
    if (TiempoAcumulado >= 0.25f)
    {
        AnguloGiro += 25.0f;
        if (AnguloGiro >= 360.0f) AnguloGiro -= 360.0f;

        BossAttackBuilder Builder(Jefe->GetWorld(), Jefe);
        float DanoBase = 400.0f, VelBase = 2400.0f;
        Builder.SetDano(DanoBase * Jefe->GetMultiplicadorDano())
            .SetVelocidad(VelBase * Jefe->GetMultiplicadorVelocidad())
            .SetEscala(1.5f);
        Builder.ConstruirMuroRadialAoE(
            Jefe->GetActorLocation(),
            8, 2, 150.0f, 3,
            AnguloGiro
        );
        TiempoAcumulado = 0.0f;
    }
}