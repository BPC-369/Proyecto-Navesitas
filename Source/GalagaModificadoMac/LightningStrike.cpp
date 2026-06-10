#include "LightningStrike.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "GalagaModificadoMacPawn.h"
#include "UObject/ConstructorHelpers.h"

ALightningStrike::ALightningStrike()
{
    PrimaryActorTick.bCanEverTick = false;

    // Esfera de colisión para detectar al jugador (radio grande)
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->SetSphereRadius(DamageRadius);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionSphere->SetGenerateOverlapEvents(true);

    // Componente Niagara
    NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
    NiagaraEffect->SetupAttachment(RootComponent);
    NiagaraEffect->bAutoActivate = false;

    // Componente de audio
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false;

    // Cargar activos desde las rutas especificadas
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraAsset(
        TEXT("/Game/Vefects/Zap_VFX/VFX/Zap/Particles/NS_Zap_05_Purple.NS_Zap_05_Purple"));
    if (NiagaraAsset.Succeeded())
        LightningEffect = NiagaraAsset.Object;

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(
        TEXT("/Game/Vefects/Zap_VFX/Audio/SFX_Vefects_Zap_Big_01.SFX_Vefects_Zap_Big_01"));
    if (SoundAsset.Succeeded())
        ImpactSound = SoundAsset.Object;
}

void ALightningStrike::BeginPlay()
{
    Super::BeginPlay();

    // Activar el efecto y el sonido de inmediato
    if (NiagaraEffect && LightningEffect)
    {
        NiagaraEffect->SetAsset(LightningEffect);
        NiagaraEffect->SetWorldScale3D(EffectScale);
        NiagaraEffect->Activate(true);
    }

    if (AudioComponent && ImpactSound)
    {
        AudioComponent->SetSound(ImpactSound);
        AudioComponent->Play();
    }

    // Vincular solapamiento para dañar al jugador
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ALightningStrike::OnOverlapBegin);

    // El rayo se autodestruye tras 2 segundos (efecto visual breve)
    SetLifeSpan(2.0f);
}

void ALightningStrike::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    // Dañar al jugador si lo toca
    AGalagaModificadoMacPawn* Jugador = Cast<AGalagaModificadoMacPawn>(OtherActor);
    if (Jugador)
    {
        UGameplayStatics::ApplyDamage(Jugador, DamageAmount, nullptr, this, UDamageType::StaticClass());
    }
}