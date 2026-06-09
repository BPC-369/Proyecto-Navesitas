#include "GalagaGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UGalagaGameInstance::SetSelectedLevel(int32 LevelIndex)
{
	SelectedLevelIndex = LevelIndex;
}

void UGalagaGameInstance::SetSelectedDifficulty(const FString& Difficulty)
{
	SelectedDifficulty = Difficulty;
}

void UGalagaGameInstance::LaunchGame()
{
	FString Options = FString::Printf(TEXT("?LevelIndex=%d?Difficulty=%s"), SelectedLevelIndex, *SelectedDifficulty);
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("escenarioEspacio"), true, Options);
}

void UGalagaGameInstance::ReturnToMainMenu()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenuLevel"));
}

void UGalagaGameInstance::RetryCurrentLevel()
{
	FString Options = FString::Printf(TEXT("?LevelIndex=%d?Difficulty=%s"), SelectedLevelIndex, *SelectedDifficulty);
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("escenarioEspacio"), true, Options);
}