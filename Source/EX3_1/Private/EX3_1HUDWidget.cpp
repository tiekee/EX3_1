#include "EX3_1HUDWidget.h"

#include "EX3_1Character.h"
#include "EX3_1GameMode.h"
#include "EX3_1PlayerController.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEX3_1HUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (RestartButton)
	{
		RestartButton->OnClicked.AddUniqueDynamic(this, &UEX3_1HUDWidget::HandleRestartClicked);
	}
	if (EndScreenContainer)
	{
		EndScreenContainer->SetVisibility(ESlateVisibility::Collapsed);
	}
	RefreshHUD();
}

void UEX3_1HUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshHUD();
}

void UEX3_1HUDWidget::RefreshHUD()
{
	const AEX3_1Character* Character = Cast<AEX3_1Character>(GetOwningPlayerPawn());
	const AEX3_1GameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AEX3_1GameMode>() : nullptr;

	if (CoinText)
	{
		const int32 Coins = Character ? Character->GetCoinCount() : 0;
		const int32 Remaining = GameMode ? GameMode->GetRemainingCoins() : 0;
		CoinText->SetText(FText::FromString(FString::Printf(TEXT("COINS  %d     WAVE LEFT  %d"), Coins, Remaining)));
	}

	if (HealthText)
	{
		const float Health = Character ? Character->GetCurrentHealth() : 0.0f;
		const float MaxHealth = Character ? Character->GetMaxHealth() : 100.0f;
		HealthText->SetText(FText::FromString(FString::Printf(TEXT("HP  %.0f / %.0f"), Health, MaxHealth)));
	}

	if (HealthBar)
	{
		const float HealthPercent = Character && Character->GetMaxHealth() > 0.0f
			? Character->GetCurrentHealth() / Character->GetMaxHealth()
			: 0.0f;
		HealthBar->SetPercent(HealthPercent);
	}

	if (ProgressText)
	{
		const int32 Level = GameMode ? GameMode->GetCurrentLevel() : 1;
		const int32 Wave = GameMode ? GameMode->GetCurrentWave() : 1;
		ProgressText->SetText(FText::FromString(FString::Printf(TEXT("LEVEL %d    WAVE %d"), Level, Wave)));
	}
}

void UEX3_1HUDWidget::ShowGameOver()
{
	ShowEndScreen(FText::FromString(TEXT("GAME OVER")), FLinearColor(1.0f, 0.08f, 0.06f));
}

void UEX3_1HUDWidget::ShowGameClear()
{
	ShowEndScreen(FText::FromString(TEXT("GAME CLEAR!")), FLinearColor(0.15f, 1.0f, 0.4f));
}

void UEX3_1HUDWidget::ShowEndScreen(const FText& Title, const FLinearColor& Color)
{
	if (EndTitleText)
	{
		EndTitleText->SetText(Title);
		EndTitleText->SetColorAndOpacity(FSlateColor(Color));
	}
	if (EndScreenContainer)
	{
		EndScreenContainer->SetVisibility(ESlateVisibility::Visible);
	}
}

void UEX3_1HUDWidget::HandleRestartClicked()
{
	if (AEX3_1PlayerController* PlayerController = Cast<AEX3_1PlayerController>(GetOwningPlayer()))
	{
		PlayerController->RestartGame();
	}
}
