#include "AttackStrategies.h"
#include "BossEstatico.h"
#include "BossAttackBuilder.h"

// PATRÓN PARED: muro gigante giratorio + domo
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

        // Muro gigante: 12 direcciones, 6 alturas, 4 columnas, espaciado 200, giro 30°
        static float AnguloGlobal = 0.0f;
        Builder.ConstruirMuroEspiral(Jefe->GetActorLocation(), 12, 6, 200.0f, 4, AnguloGlobal);
        AnguloGlobal += 30.0f;

        // Domo superior: 5 anillos, 16 proyectiles/anillo, con ralentización
        Builder.ConstruirDomoDeMuros(Jefe->GetActorLocation(), 5, 16, true);

        TiempoAcumulado = 0.0f;
    }
}

// PATRÓN ONDA EXPANSIVA: muro circular masivo con efecto de onda (reemplaza al anillo)
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

        // Muro circular expansivo: 36 direcciones, 6 alturas, 2 columnas, sin giro, con onda
        Builder.ConstruirMuroOndaExpansiva(Jefe->GetActorLocation(), 36, 6, 180.0f, 2);

        // Domo superior (4 anillos, 14 proyectiles/anillo, con ralentización)
        Builder.ConstruirDomoDeMuros(Jefe->GetActorLocation(), 4, 14, true);

        TiempoAcumulado = 0.0f;
    }
}

// PATRÓN LÁTIGO: muro giratorio rápido + domo pequeño
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

        // Muro rápido: 8 direcciones, 3 alturas, 2 columnas, espaciado 180, ángulo variable
        Builder.ConstruirMuroEspiral(Jefe->GetActorLocation(), 8, 3, 180.0f, 2, AnguloGiro);

        // Domo ligero (3 anillos, 10 proyectiles/anillo, sin ralentización)
        Builder.ConstruirDomoDeMuros(Jefe->GetActorLocation(), 3, 10, false);

        TiempoAcumulado = 0.0f;
    }
}