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

	float HacerDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);
	void Morir();
	virtual void BeginPlay() override;

	float VidaMaxima;
	float VidaActual;
	float EscudoMaximo;
	float EscudoActual;

	// Multiplicador de daño que ajusta la dificultad
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dificultad")
		float MultiplicadorDano = 1.0f;
};