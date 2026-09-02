// Copyright Epic Games, Inc. All Rights Reserved.

#include "EX3_1GameMode.h"

#include "EX3_1.h"
#include "EX3_1Character.h"
#include "EX3_1PlayerController.h"
#include "BaseItem.h"
#include "CoinItem.h"
#include "PotionItem.h"
#include "TrapItem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEX3_1GameMode::AEX3_1GameMode()
{
	CoinItemClass = ACoinItem::StaticClass();
	TrapItemClass = ATrapItem::StaticClass();
	PotionItemClass = APotionItem::StaticClass();
}

void AEX3_1GameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UClass* BlueprintClass = LoadClass<ACoinItem>(nullptr,
		TEXT("/Game/Blueprints/BP_CoinItem.BP_CoinItem_C")))
	{
		CoinItemClass = BlueprintClass;
	}
	if (UClass* BlueprintClass = LoadClass<ATrapItem>(nullptr,
		TEXT("/Game/Blueprints/BP_TrapItem.BP_TrapItem_C")))
	{
		TrapItemClass = BlueprintClass;
	}
	if (UClass* BlueprintClass = LoadClass<APotionItem>(nullptr,
		TEXT("/Game/Blueprints/BP_PotionItem.BP_PotionItem_C")))
	{
		PotionItemClass = BlueprintClass;
	}

	GetWorldTimerManager().SetTimerForNextTick(this, &AEX3_1GameMode::StartWave);
}

void AEX3_1GameMode::StartWave()
{
	if (bGameEnded)
	{
		return;
	}

	AEX3_1Character* PlayerCharacter = Cast<AEX3_1Character>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter)
	{
		GetWorldTimerManager().SetTimer(WaveStartTimer, this, &AEX3_1GameMode::StartWave, 0.25f, false);
		return;
	}

	ClearSpawnedItems();
	SpawnWaveItems(PlayerCharacter);
	bWaveTransitioning = false;

	const FString WaveMessage = FString::Printf(TEXT("LEVEL %d  -  WAVE %d START!"), CurrentLevel, CurrentWave);
	UE_LOG(LogEX3_1, Log, TEXT("%s Coins=%d Traps=%d Potions=%d"), *WaveMessage,
		TotalCoinsThisWave,
		BaseTrapCount + (CurrentLevel - 1) * TrapsAddedPerLevel + (CurrentWave - 1) * TrapsAddedPerWave,
		PotionsPerWave);
}

void AEX3_1GameMode::SpawnWaveItems(AEX3_1Character* PlayerCharacter)
{
	const int32 RequestedCoins = BaseCoinCount
		+ (CurrentLevel - 1) * CoinsAddedPerLevel
		+ (CurrentWave - 1) * CoinsAddedPerWave;
	const int32 RequestedTraps = BaseTrapCount
		+ (CurrentLevel - 1) * TrapsAddedPerLevel
		+ (CurrentWave - 1) * TrapsAddedPerWave;

	TotalCoinsThisWave = 0;
	for (int32 Index = 0; Index < RequestedCoins; ++Index)
	{
		if (SpawnItem(CoinItemClass, PlayerCharacter))
		{
			++TotalCoinsThisWave;
		}
	}

	for (int32 Index = 0; Index < RequestedTraps; ++Index)
	{
		SpawnItem(TrapItemClass, PlayerCharacter);
	}

	for (int32 Index = 0; Index < PotionsPerWave; ++Index)
	{
		SpawnItem(PotionItemClass, PlayerCharacter);
	}

	RemainingCoins = TotalCoinsThisWave;
}

ABaseItem* AEX3_1GameMode::SpawnItem(TSubclassOf<ABaseItem> ItemClass, AEX3_1Character* PlayerCharacter)
{
	if (!ItemClass || !PlayerCharacter || !GetWorld())
	{
		return nullptr;
	}

	FVector SpawnLocation;
	if (!FindSpawnLocation(PlayerCharacter, SpawnLocation))
	{
		const float FallbackOffset = 250.0f + SpawnedItems.Num() * 110.0f;
		SpawnLocation = PlayerCharacter->GetActorLocation()
			+ PlayerCharacter->GetActorForwardVector() * FallbackOffset;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = PlayerCharacter;

	ABaseItem* SpawnedItem = GetWorld()->SpawnActor<ABaseItem>(
		ItemClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	if (SpawnedItem)
	{
		SpawnedItems.Add(SpawnedItem);
	}
	return SpawnedItem;
}

bool AEX3_1GameMode::FindSpawnLocation(AEX3_1Character* PlayerCharacter, FVector& OutLocation) const
{
	if (!PlayerCharacter || !GetWorld())
	{
		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ItemSpawnGroundTrace), false, PlayerCharacter);
	for (int32 Attempt = 0; Attempt < 16; ++Attempt)
	{
		const float Angle = FMath::FRandRange(0.0f, UE_TWO_PI);
		const float Radius = FMath::FRandRange(MinimumSpawnRadius, MaximumSpawnRadius);
		const FVector Candidate = PlayerCharacter->GetActorLocation()
			+ FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * Radius;

		FHitResult GroundHit;
		const FVector TraceStart = Candidate + FVector::UpVector * 1500.0f;
		const FVector TraceEnd = Candidate - FVector::UpVector * 2000.0f;
		if (!GetWorld()->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			continue;
		}

		const FVector CandidateLocation = GroundHit.ImpactPoint + FVector::UpVector * ItemHeightAboveGround;
		bool bHasEnoughSpacing = FVector::DistSquared2D(CandidateLocation, PlayerCharacter->GetActorLocation())
			>= FMath::Square(MinimumSpawnRadius * 0.8f);
		for (const TWeakObjectPtr<ABaseItem>& ExistingItem : SpawnedItems)
		{
			if (ExistingItem.IsValid()
				&& FVector::DistSquared2D(CandidateLocation, ExistingItem->GetActorLocation()) < FMath::Square(180.0f))
			{
				bHasEnoughSpacing = false;
				break;
			}
		}

		if (bHasEnoughSpacing)
		{
			OutLocation = CandidateLocation;
			return true;
		}
	}

	return false;
}

void AEX3_1GameMode::HandleCoinCollected()
{
	if (bGameEnded || bWaveTransitioning || RemainingCoins <= 0)
	{
		return;
	}

	--RemainingCoins;
	if (RemainingCoins > 0)
	{
		return;
	}

	bWaveTransitioning = true;
	if (AEX3_1Character* PlayerCharacter = Cast<AEX3_1Character>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		PlayerCharacter->RestoreFullHealth();
	}
	ClearSpawnedItems();

	if (CurrentLevel >= MaxLevels && CurrentWave >= WavesPerLevel)
	{
		CompleteGame();
		return;
	}

	if (CurrentWave < WavesPerLevel)
	{
		++CurrentWave;
	}
	else
	{
		++CurrentLevel;
		CurrentWave = 1;
	}

	GetWorldTimerManager().SetTimer(WaveStartTimer, this, &AEX3_1GameMode::StartWave, 1.5f, false);
}

void AEX3_1GameMode::HandlePlayerDeath(AEX3_1Character* DeadCharacter)
{
	if (bGameEnded)
	{
		return;
	}

	bGameEnded = true;
	GetWorldTimerManager().ClearTimer(WaveStartTimer);
	if (AEX3_1PlayerController* PlayerController = Cast<AEX3_1PlayerController>(
		UGameplayStatics::GetPlayerController(this, 0)))
	{
		PlayerController->ShowGameOverScreen();
	}
	UGameplayStatics::SetGamePaused(this, true);
	UE_LOG(LogEX3_1, Log, TEXT("GAME OVER at Level %d Wave %d"), CurrentLevel, CurrentWave);
}

void AEX3_1GameMode::CompleteGame()
{
	bGameEnded = true;
	if (AEX3_1PlayerController* PlayerController = Cast<AEX3_1PlayerController>(
		UGameplayStatics::GetPlayerController(this, 0)))
	{
		PlayerController->ShowGameClearScreen();
	}
	UGameplayStatics::SetGamePaused(this, true);
	UE_LOG(LogEX3_1, Log, TEXT("GAME CLEAR - Level %d Wave %d completed"), CurrentLevel, CurrentWave);
}

void AEX3_1GameMode::ClearSpawnedItems()
{
	for (const TWeakObjectPtr<ABaseItem>& Item : SpawnedItems)
	{
		if (Item.IsValid())
		{
			Item->Destroy();
		}
	}
	SpawnedItems.Reset();
}
