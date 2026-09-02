#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "TrapItem.generated.h"

class AEX3_1Character;

UCLASS()
class EX3_1_API ATrapItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ATrapItem();

protected:
	virtual void BeginPlay() override;
	virtual void OnItemOverlap(AActor* OverlapActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "0.1"))
	float ActivationDelay = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "0.0"))
	float DamageAmount = 30.0f;

private:
	void TriggerTrap();

	FTimerHandle ActivationTimer;
	TWeakObjectPtr<AEX3_1Character> TriggeringCharacter;
	bool bArmed = false;
};
