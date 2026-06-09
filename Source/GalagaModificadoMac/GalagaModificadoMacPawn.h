// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GalagaModificadoMacPawn.generated.h"

class UStaticMesh;
class UComponenteCombate;
class USkeletalMeshComponent;

// --- PATRÓN STATE ---
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

// --- PATRÓN DECORATOR ---
class FDecoradorBonificacion : public IEstadoNave
{
protected:
    IEstadoNave* EstadoEnvuelto;

public:
    FDecoradorBonificacion(IEstadoNave* Estado) : EstadoEnvuelto(Estado) {}

    virtual ~FDecoradorBonificacion() {
        if (EstadoEnvuelto) delete EstadoEnvuelto;
    }

    virtual void EjecutarTransformacion(AGalagaModificadoMacPawn* NaveContexto) override {
        if (EstadoEnvuelto) EstadoEnvuelto->EjecutarTransformacion(NaveContexto);
    }
    virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) override {
        if (EstadoEnvuelto) EstadoEnvuelto->EjecutarAtaque(NaveContexto, FireDirection);
    }
};

// Bonificaciones de la Nave
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
    virtual void EjecutarAtaque(AGalagaModificadoMacPawn* NaveContexto, FVector FireDirection) override;
};

class FDecoradorSuperBuffoNave : public FDecoradorBonificacion {
public:
    FDecoradorSuperBuffoNave(IEstadoNave* Estado, AGalagaModificadoMacPawn* Contexto);
};

// Bonificaciones del Robot
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

// --- PAWN PRINCIPAL ---
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

    UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
        USkeletalMeshComponent* RobotMeshComponent;

public:
    AGalagaModificadoMacPawn();

    UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
        FVector GunOffset;

    UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
        float FireRate;

    UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
        float MoveSpeed;

    // Inclinación visual de la nave
    UPROPERTY(EditAnywhere, Category = "Movimiento")
        float MaxInclinacion = 15.0f;

    UPROPERTY(EditAnywhere, Category = "Movimiento")
        float VelocidadInclinacion = 5.0f;

    UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
        class USoundBase* FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
        TSubclassOf<class UUserWidget> WidgetGameOverClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combate")
        UComponenteCombate* ComponenteCombate;

    // --- ANIMACIONES DEL ROBOT ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animaciones Robot")
        class UAnimMontage* MontajeAtaqueNormal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animaciones Robot")
        class UAnimMontage* MontajeAtaqueRaro;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animaciones Robot")
        class UAnimMontage* MontajeAtaqueCargado;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animaciones Robot")
        class UAnimMontage* MontajeDisparoQ;

    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    void FireShot(FVector FireDirection);
    void ShotTimerExpired();
    void EmpezarDisparo();
    void DetenerDisparo();

    void IniciarCorrer();
    void DetenerCorrer();
    void AtaqueSecundario();
    void DisparoEspecial();
    void EjecutarSalto();

    bool bEstaDisparando;
    float MultiplicadorDanio;

    float TiempoDisparoCuadruple;
    int32 BombasRacimoRestantes;
    float TiempoBuffoNave;
    float VelocidadOriginalNave;

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
    bool bMuerto = false;

    UStaticMesh* RopaNave;
    UStaticMesh* RopaCubo;

public:
    FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
    FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    FORCEINLINE class USkeletalMeshComponent* GetRobotMeshComponent() const { return RobotMeshComponent; }

    void Transformar();
    void ManejarMuerte();
    void InvertirMouseFPS(float Valor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Muerte")
        void OnDeathEvent();

    UFUNCTION(BlueprintCallable, Category = "Combate")
        float GetVidaActual() const;

    UFUNCTION(BlueprintCallable, Category = "Combate")
        float GetVidaMaxima() const;

    IEstadoNave* EstadoActual;
    void CambiarEstado(IEstadoNave* NuevoEstado);
    void ConvertirEnNave();
    void ConvertirEnRobot();

    // ========== HUD ==========
    UPROPERTY()
        TMap<AActor*, class UUserWidget*> EnemyHealthWidgets;
    UPROPERTY()
        class UUserWidget* EnemyListWidget;
    UPROPERTY()
        class UUserWidget* BossHealthWidget;
    UPROPERTY()
        class UUserWidget* CrosshairWidget;
    UPROPERTY()
        class UMaterialParameterCollection* MPC_Player;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<class UUserWidget> EnemyHealthBarClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<class UUserWidget> EnemyListClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<class UUserWidget> BossHealthClass;
    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<class UUserWidget> CrosshairClass;

    void UpdateHealthBars(float DeltaSeconds);
    class UUserWidget* CreateHealthBarForEnemy(AActor* Enemy);

    bool bEnemyTargeted = false;
    // =======================

    // ========== PAUSA ==========
    UPROPERTY(EditDefaultsOnly, Category = "UI")
        TSubclassOf<class UUserWidget> PauseMenuClass;

    UPROPERTY()
        class UUserWidget* PauseMenuWidget;

    bool bIsPaused = false;

    void OnPauseButtonPressed();
    void ResumeGame();
    void ReturnToMainMenuFromPause();

    UFUNCTION(BlueprintCallable)
        void PauseResumeGame();

    UFUNCTION(BlueprintCallable)
        void PauseReturnToMainMenu();
    // ===========================

protected:
    virtual void BeginPlay() override;
    virtual void Destroyed() override;
};