#pragma once

#include "CoreMinimal.h"

class ABossEstatico;

class IAttackStrategy
{
public:
    virtual ~IAttackStrategy() = default;
    virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) = 0;
};

class FAtaqueParedStrategy : public IAttackStrategy
{
public:
    FAtaqueParedStrategy() : TiempoAcumulado(0.0f), ContadorAtaques(0) {}
    virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) override;
private:
    float TiempoAcumulado;
    int32 ContadorAtaques;
};

class FAtaqueOndaStrategy : public IAttackStrategy
{
public:
    FAtaqueOndaStrategy() : TiempoAcumulado(0.0f), ContadorAtaques(0) {}
    virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) override;
private:
    float TiempoAcumulado;
    int32 ContadorAtaques;
};

class FAtaqueLatigoStrategy : public IAttackStrategy
{
public:
    FAtaqueLatigoStrategy() : TiempoAcumulado(0.0f), AnguloGiro(0.0f), ContadorAtaques(0) {}
    virtual void Ejecutar(ABossEstatico* Jefe, float DeltaTime) override;
private:
    float TiempoAcumulado;
    float AnguloGiro;
    int32 ContadorAtaques;
};