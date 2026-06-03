#include "BossStates.h"
#include "BossEstatico.h"
#include "AttackStrategies.h"

void FFase1State::EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) { Jefe->EjecutarAtaqueEstrategico(DeltaTime); }
void FFase2State::EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) { Jefe->EjecutarAtaqueEstrategico(DeltaTime); }
void FFase3State::EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) { Jefe->EjecutarAtaqueEstrategico(DeltaTime); }

void FFase4State::EjecutarEstado(ABossEstatico* Jefe, float DeltaTime)
{
    TiempoCambio += DeltaTime;
    if (TiempoCambio >= 10.0f) // Rotación caótica de estrategias cada 10s
    {
        int Rand = FMath::RandRange(1, 3);
        if (Rand == 1) Jefe->CambiarEstrategia(new FAtaqueParedStrategy());
        else if (Rand == 2) Jefe->CambiarEstrategia(new FAtaqueOndaStrategy());
        else Jefe->CambiarEstrategia(new FAtaqueLatigoStrategy());
        TiempoCambio = 0.0f;
    }
    Jefe->EjecutarAtaqueEstrategico(DeltaTime);
}