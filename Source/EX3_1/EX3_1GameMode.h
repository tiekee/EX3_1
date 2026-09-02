// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EX3_1GameMode.generated.h"

class ABaseItem;
class ACoinItem;
class AEX3_1Character;
class APotionItem;
class ATrapItem;

UCLASS(abstract)
class AEX3_1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEX3_1GameMode();

	void HandleCoinCollected();
	void HandlePlayerDeath(AEX3_1Character* DeadCharacter);

	UFUNCTION(BlueprintPure, Category = "Progress")
	int32 GetCurrentLevel() const { return CurrentLevel; }

	UFUNCTION(BlueprintPure, Category = "Progress")
	int32 GetCurrentWave() const { return CurrentWave; }

	UFUNCTION(BlueprintPure, Category = "Progress")
	int32 GetRemainingCoins() const { return RemainingCoins; }

	UFUNCTION(BlueprintPure, Category = "Progress")
	int32 GetTotalCoinsThisWave() const { return TotalCoinsThisWave; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<ACoinItem> CoinItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<ATrapItem> TrapItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<APotionItem> PotionItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Progress", meta = (ClampMin = "1"))
	int32 MaxLevels = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Progress", meta = (ClampMin = "1"))
	int32 WavesPerLevel = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Difficulty|Coins", meta = (ClampMin = "1"))
	int32 BaseCoinCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Difficulty|Coins", meta = (ClampMin = "0"))
	int32 CoinsAddedPerLevel = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Difficulty|Coins", meta = (ClampMin = "0"))
	int32 CoinsAddedPerWave = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Difficulty|Traps", meta = (ClampMin = "0"))
	int32 BaseTrapCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Difficulty|Traps", meta = (ClampMin = "0"))
	int32 TrapsAddedPerLevel = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Difficulty|Traps", meta = (ClampMin = "0"))
	int32 TrapsAddedPerWave = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Difficulty|Potions", meta = (ClampMin = "0"))
	int32 PotionsPerWave = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "100.0"))
	float MinimumSpawnRadius = 350.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "200.0"))
	float MaximumSpawnRadius = 1400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning", meta = (ClampMin = "0.0"))
	float ItemHeightAboveGround = 65.0f;

private:
	void StartWave();
	void SpawnWaveItems(AEX3_1Character* PlayerCharacter);
	ABaseItem* SpawnItem(TSubclassOf<ABaseItem> ItemClass, AEX3_1Character* PlayerCharacter);
	bool FindSpawnLocation(AEX3_1Character* PlayerCharacter, FVector& OutLocation) const;
	void ClearSpawnedItems();
	void CompleteGame();

	int32 CurrentLevel = 1;
	int32 CurrentWave = 1;
	int32 RemainingCoins = 0;
	int32 TotalCoinsThisWave = 0;
	bool bWaveTransitioning = false;
	bool bGameEnded = false;

	FTimerHandle WaveStartTimer;
	TArray<TWeakObjectPtr<ABaseItem>> SpawnedItems;
};
