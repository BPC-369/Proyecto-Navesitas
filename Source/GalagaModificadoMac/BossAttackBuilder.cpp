#include "BossAttackBuilder.h"
#include "BossProjectileFactory.h"
#include "Math/UnrealMathUtility.h"

BossAttackBuilder::BossAttackBuilder(UWorld* EnMundo, AActor* EnDisparador)
    : Mundo(EnMundo), Disparador(EnDisparador), Dano(50.0f), Velocidad(3000.0f), Escala(1.0f) {}

BossAttackBuilder& BossAttackBuilder::SetDano(float NuevoDano) { Dano = NuevoDano; return *this; }
BossAttackBuilder& BossAttackBuilder::SetVelocidad(float NuevaVelocidad) { Velocidad = NuevaVelocidad; return *this; }
BossAttackBuilder& BossAttackBuilder::SetEscala(float NuevaEscala) { Escala = NuevaEscala; return *this; }

void BossAttackBuilder::ConstruirPared(FVector Origen, FVector DireccionFrente, int Filas, int Columnas, float Espaciado)
{
    // Calculamos los ejes laterales y verticales basados en hacia dónde mira el jefe
    FVector EjeDerecho = FVector::CrossProduct(FVector::UpVector, DireccionFrente).GetSafeNormal();
    FVector EjeArriba = FVector::CrossProduct(DireccionFrente, EjeDerecho).GetSafeNormal();

    for (int r = 0; r < Filas; ++r)
    {
        for (int c = 0; c < Columnas; ++c)
        {
            // Centramos la grilla
            float OffsetY = (c - (Columnas / 2.0f)) * Espaciado;
            float OffsetZ = (r - (Filas / 2.0f)) * Espaciado;

            FVector PosicionSpawn = Origen + (EjeDerecho * OffsetY) + (EjeArriba * OffsetZ);

            BossProjectileFactory::CrearProyectil(Mundo, PosicionSpawn, DireccionFrente.Rotation(), Disparador, Escala, Dano, Velocidad);
        }
    }
}

void BossAttackBuilder::ConstruirAbanico(FVector Origen, FVector DireccionFrente, int Cantidad, float AnguloApertura, bool bMulticapa)
{
    float PasoAngulo = AnguloApertura / FMath::Max(1, Cantidad - 1);
    float AnguloInicial = -(AnguloApertura / 2.0f);

    for (int i = 0; i < Cantidad; ++i)
    {
        float AnguloActual = AnguloInicial + (i * PasoAngulo);
        FVector DireccionDisparo = DireccionFrente.RotateAngleAxis(AnguloActual, FVector::UpVector);

        BossProjectileFactory::CrearProyectil(Mundo, Origen, DireccionDisparo.Rotation(), Disparador, Escala, Dano, Velocidad);

        // Si es multicapa, lanza otra fila idéntica justo debajo
        if (bMulticapa)
        {
            BossProjectileFactory::CrearProyectil(Mundo, Origen - FVector(0, 0, 150.0f), DireccionDisparo.Rotation(), Disparador, Escala, Dano, Velocidad);
        }
    }
}