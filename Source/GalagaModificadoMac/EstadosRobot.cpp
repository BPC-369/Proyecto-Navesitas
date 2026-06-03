#include "EstadosRobot.h"
#include "RobotFrancotirador.h"

EstadoRobot* EstadoDeteccion::Ejecutar(ARobotFrancotirador* Robot)
{
    Robot->Mover();
    if (Robot->CalcularDistanciaAlJugador() <= Robot->RangoPreparacion)
        return new EstadoPreparacion();
    return nullptr;
}

EstadoRobot* EstadoPreparacion::Ejecutar(ARobotFrancotirador* Robot)
{
    Robot->ApuntarAlJugador();
    float Distancia = Robot->CalcularDistanciaAlJugador();
    if (Distancia <= Robot->RangoDisparo)
        return new EstadoDisparo();
    else if (Distancia > Robot->RangoPreparacion)
        return new EstadoDeteccion();
    return nullptr;
}

EstadoRobot* EstadoDisparo::Ejecutar(ARobotFrancotirador* Robot)
{
    Robot->ApuntarAlJugador();
    float TiempoActual = Robot->GetWorld()->GetTimeSeconds();
    if (TiempoActual - Robot->TiempoUltimoDisparo >= Robot->TiempoEntreDisparos)
    {
        Robot->Atacar();
        Robot->TiempoUltimoDisparo = TiempoActual;
    }
    if (Robot->CalcularDistanciaAlJugador() > Robot->RangoDisparo)
        return new EstadoPreparacion();
    return nullptr;
}