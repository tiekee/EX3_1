// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface1.generated.h"


UINTERFACE(MinimalAPI)
class UItemInterface1 : public UInterface
{
	GENERATED_BODY()
};

class EX3_1_API IItemInterface1
{
	GENERATED_BODY()

public:
protected:
	virtual void OnItemOverlap(AActor* OverlapActor) = 0;
	virtual void OnItemEndOverlap(AActor* OverlapActor) = 0;
	virtual void ActiveItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;
};
