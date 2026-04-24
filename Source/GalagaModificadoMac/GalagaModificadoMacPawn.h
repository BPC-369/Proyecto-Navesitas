// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EntidadCombate.h" // 1. INCLUIMOS A LA CLASE PADRE
#include "GalagaModificadoMacPawn.generated.h"

class UStaticMesh;

// --- INICIO DEL PATRÓN STATE ---
// Declaración anticipada para que los estados sepan qué es tu nave
class AGalagaModificadoMacPawn;

// Creamos la clase Abstracta de Estado, 
class IEstadoNave
{
public:
	virtual ~IEstadoNave() = default;// Es un destructor que se encargara de limpiar la memoria cuando se destruya un estado

	// Creamos Virtuales Puros para que las clases hijas se vean obligadas a implementar estos metodos
	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) = 0;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto) = 0;
};

// Creamos la clase de Estado de NaveVoladora(Forma Nave), que heredara de la clase abstracta
class FEstadoNaveVoladora : public IEstadoNave
{
public:
	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto) override;
};

// Creamos la clase de Estado de NaveRobot(Forma Cubo), que heredara de la clase abstracta
class FEstadoNaveRobot : public IEstadoNave
{
public:
	virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override;
	virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto) override; 
};
// --- FIN DEL PATRÓN STATE ---

UCLASS(Blueprintable)
class AGalagaModificadoMacPawn : public AEntidadCombate // HEREDAMOS DE LA NUEVA CLASE
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	AGalagaModificadoMacPawn();

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	FVector GunOffset;

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);
	void Disparar();

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	// Static names for axis bindings
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName MoveUpBinding; // eje Z
	static const FName FireForwardBinding;
	static const FName FireRightBinding;

private:

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	UStaticMesh* RopaNave;
	UStaticMesh* RopaCubo;

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Función para cambiar la malla
	void Transformar();

	// AÑADIMOS ESTO: El puntero que guarda nuestro cerebro actual
	IEstadoNave* EstadoActual;

	// Función para cambiar de estado destruyendo el viejo de la memoria
	void CambiarEstado(IEstadoNave* NuevoEstado);

	// Herramientas públicas para que los estados le cambien la ropa a la nave
	void ConvertirEnNave();
	void ConvertirEnRobot();
};