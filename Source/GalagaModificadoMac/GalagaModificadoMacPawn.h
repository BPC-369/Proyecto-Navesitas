// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GalagaModificadoMacPawn.generated.h"

class UStaticMesh;
class UComponenteCombate;

// --- INICIO DEL PATRÓN STATE ---
class AGalagaModificadoMacPawn;

class IEstadoNave
{
public:
	virtual ~IEstadoNave() = default;

	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) = 0;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto) = 0;
};

class FEstadoNaveVoladora : public IEstadoNave
{
public:
	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto) override;
};

class FEstadoNaveRobot : public IEstadoNave
{
public:
	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto) override;
};
// --- FIN DEL PATRÓN STATE ---

UCLASS(Blueprintable)
class AGalagaModificadoMacPawn : public ACharacter
{
	GENERATED_BODY()

		UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* ShipMeshComponent;

	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;

	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

public:
	AGalagaModificadoMacPawn();

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		FVector GunOffset;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float MoveSpeed;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
		class USoundBase* FireSound;

	// Widget de Game Over
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
		TSubclassOf<class UUserWidget> WidgetGameOverClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate")
		UComponenteCombate* ComponenteCombate;

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void FireShot(FVector FireDirection);
	void Disparar();
	void ShotTimerExpired();
	void EmpezarDisparo();
	void DetenerDisparo();

	bool bEstaDisparando;
	float MultiplicadorDanio;

	// Nave
	float TiempoDisparoCuadruple;
	int32 BombasRacimoRestantes;
	float TiempoBuffoNave;
	float VelocidadOriginalNave;

	// Robot
	float TiempoBuffoRobot;
	float TiempoCortesDistancia;
	float TiempoInmunidad;

	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName MoveUpBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

private:
	uint32 bCanFire : 1;
	FTimerHandle TimerHandle_ShotTimerExpired;

	bool bMuerto = false;   // <- Añadido para el Game Over

	UStaticMesh* RopaNave;
	UStaticMesh* RopaCubo;

public:
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	void Transformar();
	void ManejarMuerte();   // <- Añadido

	IEstadoNave* EstadoActual;

	void CambiarEstado(IEstadoNave* NuevoEstado);

	void ConvertirEnNave();
	void ConvertirEnRobot();

protected:
	virtual void BeginPlay() override;
};