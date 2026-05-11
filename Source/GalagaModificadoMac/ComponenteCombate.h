#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComponenteCombate.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GALAGAMODIFICADOMAC_API UComponenteCombate : public UActorComponent
{
	GENERATED_BODY()

public:
	UComponenteCombate();

	FName Faccion;
	float ObtenerPorcentajeVida() const;
	float ObtenerPorcentajeEscudo() const;

	// Quitamos el 'override' porque esta función es exclusiva de tu componente
	float HacerDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	void Morir();
	virtual void BeginPlay() override;
	float VidaMaxima;
	float VidaActual;
	float EscudoMaximo;
	float EscudoActual;
};