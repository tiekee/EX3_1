#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EX3_1HUDWidget.generated.h"

class UBorder;
class UButton;
class UProgressBar;
class UTextBlock;

UCLASS()
class EX3_1_API UEX3_1HUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowGameOver();
	void ShowGameClear();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ProgressText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> EndScreenContainer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EndTitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;

private:
	UFUNCTION()
	void HandleRestartClicked();

	void RefreshHUD();
	void ShowEndScreen(const FText& Title, const FLinearColor& Color);
};
