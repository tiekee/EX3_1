#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PotionItem.generated.h"

UCLASS()
class EX3_1_API APotionItem : public ABaseItem
{
	GENERATED_BODY()

public:
	APotionItem();
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	virtual void ActiveItem(AActor* Activator) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "0.0"))
	float HealAmount = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float RotationSpeed = 60.0f;

private:
	bool bConsumed = false;
};
