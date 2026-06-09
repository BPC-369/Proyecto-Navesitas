#include "BossAttackBuilder.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "BossEstatico.h"

BossAttackBuilder::BossAttackBuilder(UWorld* InWorld, AActor* InDueno)
    : Mundo(InWorld), Dueno(InDueno), Dano(80.0f), Velocidad(1900.0f), Escala(2.0f) {}

BossAttackBuilder& BossAttackBuilder::SetDano(float InDano) { Dano = InDano; return *this; }
BossAttackBuilder& BossAttackBuilder::SetVelocidad(float InVelocidad) { Velocidad = InVelocidad; return *this; }
BossAttackBuilder& BossAttackBuilder::SetEscala(float InEscala) { Escala = InEscala; return *this; }

void BossAttackBuilder::ObtenerDimensionesJefe(float& Radio, float& MitadAltura) const
{
    Radio = 0.0f;
    MitadAltura = 0.0f;

    if (!Dueno) return;
    ABossEstatico* Jefe = Cast<ABossEstatico>(Dueno);
    if (!Jefe) return;

    if (UCapsuleComponent* Capsula = Cast<UCapsuleComponent>(Jefe->GetRootComponent()))
    {
        Radio = Capsula->GetScaledCapsuleRadius();
        MitadAltura = Capsula->GetScaledCapsuleHalfHeight();
    }
}

// ===== MÉTODOS ANTIGUOS (conservados) =====
void BossAttackBuilder::ConstruirMuroRadialAoE(FVector Origen, int32 CantidadDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas, float AnguloBase)
{
    if (!Mundo || CantidadDirecciones <= 0 || AlturaMuro <= 0 || Columnas <= 0) return;

    float RadioCapsula = 0.0f, MitadAltura = 0.0f;
    ObtenerDimensionesJefe(RadioCapsula, MitadAltura);
    if (RadioCapsula <= 0.0f) return;

    const float RadioSeguro = RadioCapsula + 100.0f;
    const float AnguloPaso = 360.0f / CantidadDirecciones;

    for (int32 d = 0; d < CantidadDirecciones; ++d)
    {
        float AnguloActual = AnguloBase + (d * AnguloPaso);
        FVector Direccion = FVector(1, 0, 0).RotateAngleAxis(AnguloActual, FVector::UpVector);
        FVector OrigenLinea = Origen + (Direccion * RadioSeguro);
        FVector Perpendicular = FVector::CrossProduct(Direccion, FVector::UpVector).GetSafeNormal();

        for (int32 h = 0; h < AlturaMuro; ++h)
        {
            FVector BaseVertical = OrigenLinea + (FVector::UpVector * h * Espaciado);

            for (int32 c = 0; c < Columnas; ++c)
            {
                float OffsetLateral = (c - (Columnas - 1) * 0.5f) * Espaciado;
                FVector PosicionSpawn = BaseVertical + (Perpendicular * OffsetLateral);

                FActorSpawnParameters Params;
                Params.Instigator = Cast<APawn>(Dueno);
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
                    ABossProjectile::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, Params);
                if (Proyectil)
                {
                    Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
                    Proyectil->SetDireccion(Direccion);
                    if (Proyectil->Colisionador)
                    {
                        Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
                    }
                }
            }
        }
    }
}

void BossAttackBuilder::ConstruirMuroEspiral(FVector Origen, int32 NumDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas, float AnguloOffset)
{
    if (!Mundo || NumDirecciones <= 0 || AlturaMuro <= 0 || Columnas <= 0) return;

    float RadioCapsula = 0.0f, MitadAltura = 0.0f;
    ObtenerDimensionesJefe(RadioCapsula, MitadAltura);
    if (RadioCapsula <= 0.0f) return;

    const float RadioSeguro = RadioCapsula + 150.0f;
    const float AnguloPaso = 360.0f / NumDirecciones;

    float AlturaMin = -MitadAltura - 50.0f;
    float AlturaMax = MitadAltura + 50.0f;

    for (int32 d = 0; d < NumDirecciones; ++d)
    {
        float AnguloActual = AnguloOffset + (d * AnguloPaso);
        FVector Direccion = FVector(1, 0, 0).RotateAngleAxis(AnguloActual, FVector::UpVector);
        FVector OrigenLinea = Origen + (Direccion * RadioSeguro);
        FVector Perpendicular = FVector::CrossProduct(Direccion, FVector::UpVector).GetSafeNormal();

        for (int32 h = 0; h < AlturaMuro; ++h)
        {
            float AlturaActual = FMath::Lerp(AlturaMin, AlturaMax, (float)h / (AlturaMuro - 1));
            FVector BaseVertical = OrigenLinea + FVector(0, 0, AlturaActual);

            for (int32 c = 0; c < Columnas; ++c)
            {
                float OffsetLateral = (c - (Columnas - 1) * 0.5f) * Espaciado;
                FVector PosicionSpawn = BaseVertical + (Perpendicular * OffsetLateral);

                FActorSpawnParameters Params;
                Params.Instigator = Cast<APawn>(Dueno);
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
                    ABossProjectile::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, Params);
                if (Proyectil)
                {
                    Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
                    Proyectil->SetDireccion(Direccion);
                    if (Proyectil->Colisionador)
                    {
                        Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
                    }
                }
            }
        }
    }
}

void BossAttackBuilder::ConstruirMuroOndaExpansiva(FVector Origen, int32 NumDirecciones, int32 AlturaMuro, float Espaciado, int32 Columnas)
{
    if (!Mundo || NumDirecciones <= 0 || AlturaMuro <= 0 || Columnas <= 0) return;

    float RadioCapsula = 0.0f, MitadAltura = 0.0f;
    ObtenerDimensionesJefe(RadioCapsula, MitadAltura);
    if (RadioCapsula <= 0.0f) return;

    const float RadioSeguro = RadioCapsula + 150.0f;
    const float AnguloPaso = 360.0f / NumDirecciones;

    float AlturaMin = -MitadAltura - 50.0f;
    float AlturaMax = MitadAltura + 50.0f;

    for (int32 d = 0; d < NumDirecciones; ++d)
    {
        float AnguloActual = d * AnguloPaso;
        FVector Direccion = FVector(1, 0, 0).RotateAngleAxis(AnguloActual, FVector::UpVector);
        FVector OrigenLinea = Origen + (Direccion * RadioSeguro);
        FVector Perpendicular = FVector::CrossProduct(Direccion, FVector::UpVector).GetSafeNormal();

        for (int32 h = 0; h < AlturaMuro; ++h)
        {
            float AlturaActual = FMath::Lerp(AlturaMin, AlturaMax, (float)h / (AlturaMuro - 1));
            FVector BaseVertical = OrigenLinea + FVector(0, 0, AlturaActual);

            for (int32 c = 0; c < Columnas; ++c)
            {
                float OffsetLateral = (c - (Columnas - 1) * 0.5f) * Espaciado;
                FVector PosicionSpawn = BaseVertical + (Perpendicular * OffsetLateral);

                FActorSpawnParameters Params;
                Params.Instigator = Cast<APawn>(Dueno);
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
                    ABossProjectile::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, Params);
                if (Proyectil)
                {
                    Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
                    Proyectil->SetDireccion(Direccion);
                    if (Proyectil->Colisionador)
                    {
                        Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
                    }

                    if (FMath::FRand() < 0.2f)
                    {
                        Proyectil->SetRalentiza(true);
                    }
                }
            }
        }
    }
}

void BossAttackBuilder::ConstruirAnilloOndaExpansiva(FVector Origen, int32 CantidadProyectiles, bool ActivarOnda)
{
    if (!Mundo || CantidadProyectiles <= 0) return;

    float RadioCapsula = 0.0f, MitadAltura = 0.0f;
    ObtenerDimensionesJefe(RadioCapsula, MitadAltura);
    if (RadioCapsula <= 0.0f) return;

    const float RadioSeguro = RadioCapsula + 150.0f;
    const float AnguloPaso = 360.0f / CantidadProyectiles;

    for (int32 i = 0; i < CantidadProyectiles; ++i)
    {
        float AnguloActual = i * AnguloPaso;
        FVector DirRotada = FVector(1, 0, 0).RotateAngleAxis(AnguloActual, FVector::UpVector);
        FVector PosicionSpawn = Origen + (DirRotada * RadioSeguro);

        FActorSpawnParameters Params;
        Params.Instigator = Cast<APawn>(Dueno);
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
            ABossProjectile::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, Params);
        if (Proyectil)
        {
            Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
            Proyectil->SetDireccion(DirRotada);
            if (Proyectil->Colisionador)
            {
                Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
            }

            if (FMath::FRand() < 0.2f)
            {
                Proyectil->SetRalentiza(true);
            }
        }
    }
}

void BossAttackBuilder::ConstruirDomoDeMuros(FVector Origen, int32 Anillos, int32 ProyectilesPorAnillo, bool ActivarRalentizacion)
{
    if (!Mundo || Anillos <= 0 || ProyectilesPorAnillo <= 0) return;

    float RadioCapsula = 0.0f, MitadAltura = 0.0f;
    ObtenerDimensionesJefe(RadioCapsula, MitadAltura);
    if (RadioCapsula <= 0.0f) return;

    const float RadioDomo = RadioCapsula + 200.0f;

    for (int32 i = 0; i <= Anillos; ++i)
    {
        float Elevacion = FMath::Lerp(10.0f, 85.0f, (float)i / Anillos);
        float Altura = Origen.Z + RadioDomo * FMath::Sin(FMath::DegreesToRadians(Elevacion));
        float RadioProyectado = RadioDomo * FMath::Cos(FMath::DegreesToRadians(Elevacion));

        FVector CentroAnillo(Origen.X, Origen.Y, Altura);

        for (int32 j = 0; j < ProyectilesPorAnillo; ++j)
        {
            float Angulo = (360.0f / ProyectilesPorAnillo) * j;
            FVector Direccion = FVector(1, 0, 0).RotateAngleAxis(Angulo, FVector::UpVector);
            FVector Posicion = CentroAnillo + (Direccion * RadioProyectado);

            FVector DireccionDisparo = (Posicion - Origen).GetSafeNormal();

            FActorSpawnParameters Params;
            Params.Instigator = Cast<APawn>(Dueno);
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
                ABossProjectile::StaticClass(), Posicion, FRotator::ZeroRotator, Params);
            if (Proyectil)
            {
                Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
                Proyectil->SetDireccion(DireccionDisparo);
                if (Proyectil->Colisionador)
                {
                    Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
                }
                if (ActivarRalentizacion && FMath::FRand() < 0.2f)
                {
                    Proyectil->SetRalentiza(true);
                }
            }
        }
    }
}

void BossAttackBuilder::ConstruirDomoDeProyectiles(FVector Origen, int32 NumProyectiles, bool ActivarRalentizacion)
{
    // Conservado, no se usa actualmente
}

// ===== NUEVO: MURO FRONTAL CON CURVATURA =====
void BossAttackBuilder::ConstruirMuroFrontal(FVector Origen, FVector DireccionObjetivo, int32 AlturaMuro, float Espaciado, int32 Columnas)
{
    if (!Mundo || AlturaMuro <= 0 || Columnas <= 0) return;

    float RadioCapsula = 0.0f, MitadAltura = 0.0f;
    ObtenerDimensionesJefe(RadioCapsula, MitadAltura);
    if (RadioCapsula <= 0.0f) return;

    const float RadioSeguro = RadioCapsula + 150.0f;
    FVector DireccionBase = DireccionObjetivo.GetSafeNormal();
    FVector Perpendicular = FVector::CrossProduct(DireccionBase, FVector::UpVector).GetSafeNormal();

    float AlturaMin = -MitadAltura - 50.0f;
    float AlturaMax = MitadAltura + 50.0f;

    float MaxCurvatura = 15.0f;

    for (int32 h = 0; h < AlturaMuro; ++h)
    {
        float AlturaActual = FMath::Lerp(AlturaMin, AlturaMax, (float)h / (AlturaMuro - 1));
        FVector BaseVertical = Origen + (DireccionBase * RadioSeguro) + FVector(0, 0, AlturaActual);

        for (int32 c = 0; c < Columnas; ++c)
        {
            float OffsetLateral = (c - (Columnas - 1) * 0.5f) * Espaciado;
            FVector PosicionSpawn = BaseVertical + (Perpendicular * OffsetLateral);

            float Normalizado = (Columnas > 1) ? (c / (float)(Columnas - 1)) : 0.5f;
            float AnguloCurvatura = FMath::Lerp(-MaxCurvatura, MaxCurvatura, Normalizado);
            FVector DireccionProyectil = DireccionBase.RotateAngleAxis(AnguloCurvatura, FVector::UpVector);

            FActorSpawnParameters Params;
            Params.Instigator = Cast<APawn>(Dueno);
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
                ABossProjectile::StaticClass(), PosicionSpawn, FRotator::ZeroRotator, Params);
            if (Proyectil)
            {
                Proyectil->ConfigurarProyectil(Velocidad, Dano, FVector(Escala));
                Proyectil->SetDireccion(DireccionProyectil);
                if (Proyectil->Colisionador)
                {
                    Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
                }
                if (FMath::FRand() < 0.2f)
                {
                    Proyectil->SetRalentiza(true);
                }
            }
        }
    }
}

// ===== NUEVO: LLUVIA TECHADA =====
void BossAttackBuilder::ConstruirLluviaTechada(FVector Origen, float Radio, int32 Densidad, float AlturaCaida)
{
    if (!Mundo || Radio <= 0.0f || Densidad <= 0) return;

    float Paso = Radio * 2.0f / FMath::Sqrt(static_cast<float>(Densidad));
    int32 PuntosPorLado = FMath::CeilToInt(Radio / Paso);

    for (int32 i = -PuntosPorLado; i <= PuntosPorLado; ++i)
    {
        for (int32 j = -PuntosPorLado; j <= PuntosPorLado; ++j)
        {
            FVector Punto(Origen.X + i * Paso, Origen.Y + j * Paso, Origen.Z + AlturaCaida);
            if (FVector::Dist2D(Punto, Origen) > Radio) continue;
            if (FMath::FRand() < 0.25f) continue;

            FActorSpawnParameters Params;
            Params.Instigator = Cast<APawn>(Dueno);
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            ABossProjectile* Proyectil = Mundo->SpawnActor<ABossProjectile>(
                ABossProjectile::StaticClass(), Punto, FRotator::ZeroRotator, Params);
            if (Proyectil)
            {
                Proyectil->ConfigurarProyectil(Velocidad * 0.5f, Dano * 0.8f, FVector(Escala * 0.7f));
                Proyectil->SetDireccion(FVector(0, 0, -1));
                if (Proyectil->Colisionador)
                {
                    Proyectil->Colisionador->MoveIgnoreActors.Add(Dueno);
                }
            }
        }
    }
}