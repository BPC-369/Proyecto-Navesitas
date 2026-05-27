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
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) = 0;
};

class FEstadoNaveVoladora : public IEstadoNave
{
public:
	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) override;
};

class FEstadoNaveRobot : public IEstadoNave
{
public:
	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) override;
};
// --- FIN DEL PATRÓN STATE ---

// --- INICIO DEL PATRÓN DECORATOR ---

// 1. EL DECORADOR BASE (La Envoltura)
class FDecoradorBonificacion : public IEstadoNave
{
protected:
	IEstadoNave* EstadoEnvuelto; // El estado o poder que estamos envolviendo

public:
	FDecoradorBonificacion(IEstadoNave* Estado) : EstadoEnvuelto(Estado) {}

	// ¡CRÍTICO PARA LA MEMORIA! Al destruirse, destruye lo que envuelve en cadena.
	virtual ~FDecoradorBonificacion() {
		if (EstadoEnvuelto) {
			delete EstadoEnvuelto;
		}
	}

	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override {
		if (EstadoEnvuelto) EstadoEnvuelto->EjecutarTransformacion(NaveContexto);
	}
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) override {
		if (EstadoEnvuelto) EstadoEnvuelto->EjecutarAtaque(NaveContexto, FireDirection);
	}
};

// ==========================================
// 2. LAS 4 BONIFICACIONES DE LA NAVE
// ==========================================

class FDecoradorRecuperacionNave : public FDecoradorBonificacion {
public:
	FDecoradorRecuperacionNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
};

class FDecoradorCuadrupleCanon : public FDecoradorBonificacion {
public:
	FDecoradorCuadrupleCanon(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) override;
};

class FDecoradorBombasRacimo : public FDecoradorBonificacion {
public:
	FDecoradorBombasRacimo(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
};

class FDecoradorSuperBuffoNave : public FDecoradorBonificacion {
public:
	FDecoradorSuperBuffoNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
};

// ==========================================
// 3. LAS 4 BONIFICACIONES DEL ROBOT (Solo Declaraciones)
// ==========================================

class FDecoradorVelocidadDash : public FDecoradorBonificacion {
public:
	FDecoradorVelocidadDash(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
};

class FDecoradorCortesDistancia : public FDecoradorBonificacion {
public:
	FDecoradorCortesDistancia(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) override;
};

class FDecoradorRecuperacionRobot : public FDecoradorBonificacion {
public:
	FDecoradorRecuperacionRobot(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
};

class FDecoradorInmunidad : public FDecoradorBonificacion {
public:
	FDecoradorInmunidad(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
};
// --- FIN DEL PATRÓN DECORATOR ---

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