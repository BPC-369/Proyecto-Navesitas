#pragma once
#include "CoreMinimal.h"

class ARobotFrancotirador;

class EstadoRobot
{
public:
    virtual ~EstadoRobot() {}
    virtual EstadoRobot* Ejecutar(ARobotFrancotirador* Robot) = 0;
};

class EstadoDeteccion : public EstadoRobot
{
public:
    virtual EstadoRobot* Ejecutar(ARobotFrancotirador* Robot) override;
};

class EstadoPreparacion : public EstadoRobot
{
public:
    virtual EstadoRobot* Ejecutar(ARobotFrancotirador* Robot) override;
};

class EstadoDisparo : public EstadoRobot
{
public:
    virtual EstadoRobot* Ejecutar(ARobotFrancotirador* Robot) override;
};