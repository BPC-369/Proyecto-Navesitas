#pragma once
#include "CoreMinimal.h"
#include "GalagaModificadoMacProjectile.h"
// Incluimos esta cabecera para los par?metros de la funci?n de solapamiento
#include "Components/PrimitiveComponent.h"
#include "ProyectilJefe.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API AProyectilJefe : public AGalagaModificadoMacProjectile
{
	GENERATED_BODY()

public:
	AProyectilJefe();

protected:
	// --- NUEVA FUNCI?N PARA DETECTAR EL SOLAPAMIENTO ---
	// La macro UFUNCTION() es OBLIGATORIA para que Unreal pueda conectarla
	UFUNCTION()
	void SobreSolapamientoMalla(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};