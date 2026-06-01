#pragma once
#include "CoreMinimal.h"

class ABossEstatico;

class IAttackStrategy
{
public:
    virtual ~IAttackStrategy() = default;
    virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) = 0;
};

class FAtaqueParedStrategy : public IAttackStrategy {
public: virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) override;
private: float TiempoAcumulado = 0.0f;
};

class FAtaqueOndaStrategy : public IAttackStrategy {
public: virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) override;
private: float TiempoAcumulado = 0.0f; int Olas = 0;
};

class FAtaqueLatigoStrategy : public IAttackStrategy {
public: virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) override;
private: float TiempoAcumulado = 0.0f; float Angulo = 0.0f;
};