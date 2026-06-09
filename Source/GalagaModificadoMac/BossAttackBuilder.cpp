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

// Muro original (sin cambios)
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

// Muro en espiral (para Pared y Látigo)
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

// Muro de onda expansiva (muro circular con efecto de onda)
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

    // Todas las direcciones a la vez (sin giro)
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

                    // Ralentización aleatoria
                    if (FMath::FRand() < 0.2f)
                    {
                        Proyectil->SetRalentiza(true);
                    }
                }
            }
        }
    }
}

// Anillo de onda expansiva (un solo proyectil por dirección, sin muro)
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

            // Ralentización aleatoria
            if (FMath::FRand() < 0.2f)
            {
                Proyectil->SetRalentiza(true);
            }
        }
    }
}

// Domo de muros (semiesfera superior)
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

// Nube aleatoria (no se usa actualmente, se conserva)
void BossAttackBuilder::ConstruirDomoDeProyectiles(FVector Origen, int32 NumProyectiles, bool ActivarRalentizacion) { /* ... */ }