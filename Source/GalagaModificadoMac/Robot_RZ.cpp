#include "Robot_RZ.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "TimerManager.h" 
#include "ComponenteCombate.h"
#include "GameFramework/CharacterMovementComponent.h"

ARobot_RZ::ARobot_RZ()
{
	PrimaryActorTick.bCanEverTick = true;

	VelocidadMovimiento = 400.0f;
	RangoAtaque = 150.0f;
	DanioBase = 20.0f;
	bEstaAtacando = false;

    if (ComponenteCombate != nullptr)
    {
        ComponenteCombate->VidaMaxima = 50.0f;
        ComponenteCombate->VidaActual = ComponenteCombate->VidaMaxima;
        ComponenteCombate->EscudoMaximo = 0.0f;
        ComponenteCombate->EscudoActual = ComponenteCombate->EscudoMaximo;
        ComponenteCombate->Faccion = FName("Enemigo");
    }

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> FormaCono(TEXT("SkeletalMesh'/Game/Geometry/RazoMelee/mallaRZ_Melee.mallaRZ_Melee'"));
    if (FormaCono.Succeeded())
    {
        GetMesh()->SetSkeletalMesh(FormaCono.Object);
        GetMesh()->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

        GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
        GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    static ConstructorHelpers::FClassFinder<UAnimInstance> AnimacionRobotAsset(TEXT("AnimBlueprint'/Game/Blueprints/ABP_RZ.ABP_RZ_C'"));

    if (AnimacionRobotAsset.Succeeded())
    {
        GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint); // Añade esta línea por seguridad
        GetMesh()->SetAnimInstanceClass(AnimacionRobotAsset.Class);
    }
}

void ARobot_RZ::Atacar()
{// Si el candado está puesto, ignoramos cualquier otra orden (Hace que sea incancelable)
    if (bEstaAtacando) return;

    bEstaAtacando = true;

    // FASE 1: INICIA LA EMBESTIDA (Mitad de velocidad)
    GetCharacterMovement()->MaxWalkSpeed = (VelocidadMovimiento / 2.0f);

    // Le damos un valor por defecto por si olvidaste poner la animación en el editor
    float DuracionExacta = 1.0f;

    if (AnimacionAtaqueMelee != nullptr)
    {
        // ¡EL TRUCO MAGICO! PlayAnimMontage nos devuelve cuánto dura la animación
        DuracionExacta = PlayAnimMontage(AnimacionAtaqueMelee);
    }

    // El temporizador ahora espera a que la animación termine COMPLETAMENTE
    GetWorld()->GetTimerManager().SetTimer(TimerAtaque, this, &ARobot_RZ::EjecutarGolpeMelee, DuracionExacta, false);
}

void ARobot_RZ::EjecutarGolpeMelee()
{
    // FASE 2: LA ANIMACIÓN TERMINÓ, CALCULAMOS EL IMPACTO
    float DistanciaActual = CalcularDistanciaAlJugador();

    // Si tú sigues en su rango exactamente en este milisegundo final, te aplica el daño
    if (DistanciaActual > 0.0f && DistanciaActual <= (RangoAtaque + 20.0f))
    {
        ACharacter* Jugador = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (Jugador)
        {
            UGameplayStatics::ApplyDamage(Jugador, DanioBase, GetInstigatorController(), this, UDamageType::StaticClass());
        }
    }

    // FASE 3: FIN ESTRICTO DEL ATAQUE
    // Le devolvemos su velocidad física (400)
    GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;

    // Quitamos el candado para que el Abuelo vuelva a tomar el control y te persiga
    bEstaAtacando = false;
}

void ARobot_RZ::ResetearAtaque()
{
    // FASE 3: FIN DEL COMBOM
        // Han pasado 1.5s totales. La animación terminó. 
        // AHORA SÍ le devolvemos su velocidad física (400) y quitamos el candado.
    GetCharacterMovement()->MaxWalkSpeed = VelocidadMovimiento;
    bEstaAtacando = false;
}