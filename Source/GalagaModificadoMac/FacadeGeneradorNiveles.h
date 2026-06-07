#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FacadeGeneradorNiveles.generated.h"

class AEscenarioBase;
class AObstaculoFactory;

USTRUCT(BlueprintType)
struct FConfiguracionNivel
{
	GENERATED_BODY()

		UPROPERTY()
		int32 TipoAmbiente;

	UPROPERTY()
		int32 CantidadObstaculosA;

	UPROPERTY()
		int32 CantidadObstaculosB;

	UPROPERTY()
		TMap<int32, int32> EnemigosPorGenerar;
};

UCLASS()
class GALAGAMODIFICADOMAC_API UFacadeGeneradorNiveles : public UObject
{
	GENERATED_BODY()

public:
	void Inicializar(UWorld* WorldContext);
	void CargarNivelPorIndice(int32 IndiceNivel);

	UPROPERTY()
		FConfiguracionNivel ConfiguracionActual;

	FConfiguracionNivel GetConfiguracionActual() const { return ConfiguracionActual; }

	void AplicarDificultad(const FString& Difficulty);

	// Este multiplicador se usa para ajustar la vida y el daño de los enemigos
	UPROPERTY(BlueprintReadOnly)
		float MultiplicadorDificultad = 1.0f;

private:
	UPROPERTY()
		UWorld* MundoActual;

	TArray<FConfiguracionNivel> CampanaNiveles;

	UPROPERTY()
		AEscenarioBase* EscenarioActivo;

	UPROPERTY()
		AObstaculoFactory* FabricaActiva;

	void InicializarCampana();
	void DestruirNivelAnterior();
};