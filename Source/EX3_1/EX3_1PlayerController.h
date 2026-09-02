// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EX3_1PlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UEX3_1HUDWidget;

UCLASS(abstract)
class AEX3_1PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEX3_1PlayerController();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartGame();

	void ShowGameOverScreen();
	void ShowGameClearScreen();

protected:
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY()
	TObjectPtr<UEX3_1HUDWidget> GameHUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UEX3_1HUDWidget> GameHUDWidgetClass;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
