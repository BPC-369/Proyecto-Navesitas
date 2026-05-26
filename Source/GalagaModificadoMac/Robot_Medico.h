#pragma once

#include "CoreMinimal.h"
#include "EnemigoTerrestre.h"
#include "Robot_Medico.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API ARobot_Medico : public AEnemigoTerrestre
{
	GENERATED_BODY()

public:
	ARobot_Medico();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	// Anulamos la IA del padre (perseguir y atacar al jugador) para poner la nuestra
	virtual void ActualizarComportamiento() override;

public:
	bool bEsElite;
	float RangoCuracionArea;

private:
	// Puntero para recordar a quién estamos curando
	AEnemigoTerrestre* AliadoObjetivo;

	// Timers para no saturar el Tick
	FTimerHandle TimerBusqueda;
	FTimerHandle TimerCuracion;
	FTimerHandle TimerPaseo;

	float TasaCuracionActual;
	bool bEstaCurando;
	FVector DestinoAleatorio;
	bool bTieneDestinoAleatorio;

	// Funciones internas del médico
	void BuscarAliado();
	void IniciarCuracion();
	void CuracionGradual();
	void DetenerCuracion();
	void CurarAreaElite();
	void GenerarDestinoAleatorio();
};