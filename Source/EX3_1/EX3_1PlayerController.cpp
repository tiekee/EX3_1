// Copyright Epic Games, Inc. All Rights Reserved.

#include "EX3_1PlayerController.h"

#include "EX3_1.h"
#include "EX3_1HUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Input/SVirtualJoystick.h"

AEX3_1PlayerController::AEX3_1PlayerController()
{
}

void AEX3_1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		UClass* WidgetClass = GameHUDWidgetClass.Get();
		if (!WidgetClass)
		{
			WidgetClass = LoadClass<UEX3_1HUDWidget>(
				nullptr, TEXT("/Game/UI/WBP_GameHUD.WBP_GameHUD_C"));
		}
		GameHUDWidget = WidgetClass ? CreateWidget<UEX3_1HUDWidget>(this, WidgetClass) : nullptr;
		if (GameHUDWidget)
		{
			GameHUDWidget->AddToPlayerScreen(10);
		}
	}

	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogEX3_1, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
}

void AEX3_1PlayerController::ShowGameOverScreen()
{
	if (GameHUDWidget)
	{
		GameHUDWidget->ShowGameOver();
	}

	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AEX3_1PlayerController::ShowGameClearScreen()
{
	if (GameHUDWidget)
	{
		GameHUDWidget->ShowGameClear();
	}

	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AEX3_1PlayerController::RestartGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
}

void AEX3_1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}
