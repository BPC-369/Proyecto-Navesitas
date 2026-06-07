#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GalagaGameInstance.generated.h"

UCLASS()
class GALAGAMODIFICADOMAC_API UGalagaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Menu")
		int32 SelectedLevelIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Menu")
		FString SelectedDifficulty;

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void SetSelectedLevel(int32 LevelIndex);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void SetSelectedDifficulty(const FString& Difficulty);

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void LaunchGame();

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void RetryCurrentLevel();
};