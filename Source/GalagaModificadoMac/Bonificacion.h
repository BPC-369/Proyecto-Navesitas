#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bonificacion.generated.h"

// Declaración anticipada de clases
class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class GALAGAMODIFICADOMAC_API ABonificacion : public AActor
{
	GENERATED_BODY()

public:
	ABonificacion();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Función nativa de C++ para colisiones (No requiere UFUNCTION)
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	static int32 CantidadActivas;

private:
	// Punteros crudos sin UPROPERTY (El motor los mantendrá vivos porque son subobjetos predeterminados)
	USphereComponent* VolumenColision;
	UStaticMeshComponent* MallaBonificacion;

	// Variables de Animación en C++ puro
	float VelocidadRotacion;
	float AmplitudFlote;
	float FrecuenciaFlote;
	FVector PosicionInicial;
};