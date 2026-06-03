#pragma once
#include "CoreMinimal.h"
class ABossEstatico;

class IBossState {
public: virtual ~IBossState() = default; virtual void EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) = 0;
};

class FFase1State : public IBossState { public: virtual void EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) override; };
class FFase2State : public IBossState { public: virtual void EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) override; };
class FFase3State : public IBossState { public: virtual void EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) override; };

class FFase4State : public IBossState {
public: virtual void EjecutarEstado(ABossEstatico* Jefe, float DeltaTime) override;
private: float TiempoCambio = 10.0f;
};