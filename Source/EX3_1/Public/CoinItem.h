// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"


UCLASS()
class EX3_1_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ACoinItem();
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void ActiveItem(AActor* Activator) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float RotationSpeed = 90.0f;

private:
	bool bCollected = false;
};
