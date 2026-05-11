#include "Robot_Lider.h"
#include "Kismet/GameplayStatics.h" // Para el ataque en área y spawn
#include "Robot_RZ.h"

ARobot_Lider::ARobot_Lider()
{
	VidaMaxima = 250.0f;    // Vida moderada (Ajustado a 250 en lugar de 25)
	EscudoMaximo = 150.0f;  // Posee un escudo
	DanioArea = 50.0f;      // Daño alto
	RadioAtaqueArea = 600.0f;

	VelocidadMovimiento = 400.0f; // Dale el valor de "Velocidad alta" que exige tu GDD
	RangoDeteccion = 1500.0f;     // Aumenta la visión para que detecte al jugador desde lejos
	RangoAtaque = 300.0f;         // Distancia a la que se detiene a atacar

	// Nota: La velocidad alta se configura en el CharacterMovement 
	// o mediante una variable si es un Pawn simple.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FormaCono(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_QuadPyramid.Shape_QuadPyramid'"));
	if (FormaCono.Succeeded() && MallaEnemiga != nullptr)
	{
		MallaEnemiga->SetStaticMesh(FormaCono.Object);

		// Rotamos el cono para que la punta mire hacia el frente (Eje X)
		MallaEnemiga->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
}

void ARobot_Lider::BeginPlay()
{
	Super::BeginPlay();

	// Iniciamos su comportamiento de Comandante 
	// Invoca refuerzos cada 15 segundos
	GetWorldTimerManager().SetTimer(TimerHandle_Invocacion, this, &ARobot_Lider::InvocarRefuerzos, 1.0f, true);

	// Ataque en área cada 8 segundos
	GetWorldTimerManager().SetTimer(TimerHandle_AtaqueArea, this, &ARobot_Lider::EjecutarAtaqueArea, 8.0f, true);
}

void ARobot_Lider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Aquí podrías añadir lógica adicional para el movimiento o comportamiento del líder
}

void ARobot_Lider::InvocarRefuerzos()
{
	// 1. Calculamos la posición, pero le sumamos 100 en Z para que nazca en el aire y no choque con el piso
	FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * 200.0f) + FVector(0.0f, 0.0f, 100.0f);
	FRotator SpawnRotation = GetActorRotation();

	// 2. Parámetros avanzados para OBLIGAR al motor a ignorar colisiones al nacer
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 3. Spawneamos (Si tienes una variable ClaseParaInvocar, úsala aquí. Si no, usamos ARobot_RZ)
	AActor* NuevoEnemigo = GetWorld()->SpawnActor<ARobot_RZ>(ARobot_RZ::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

	// 4. Prueba de diagnóstico en pantalla
	if (NuevoEnemigo != nullptr)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("¡ROBOT RAZO INVOCADO!"));
		}
	}
}

void ARobot_Lider::EjecutarAtaqueArea()
{
	// Buscamos a todos los actores en el radio de ataque
	TArray<AActor*> ActoresAlrededor;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), RadioAtaqueArea, ObjectTypes, nullptr, TArray<AActor*>(), ActoresAlrededor);

	for (AActor* Actor : ActoresAlrededor)
	{
		// Solo hacemos daño si es un enemigo (distinta facción)
		AEntidadCombate* Entidad = Cast<AEntidadCombate>(Actor);
		if (Entidad && Entidad->Faccion != this->Faccion)
		{
			UGameplayStatics::ApplyDamage(Entidad, DanioArea, GetController(), this, UDamageType::StaticClass());
		}
	}
}