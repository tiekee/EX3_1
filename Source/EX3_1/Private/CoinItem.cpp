// Fill out your copyright notice in the Description page of Project Settings.


#include "CoinItem.h"
#include "EX3_1Character.h"
#include "EX3_1GameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

ACoinItem::ACoinItem()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemType = TEXT("Coin");
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInterface* BaseMaterial = ItemMesh->GetMaterial(0))
	{
		UMaterialInstanceDynamic* CoinMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		CoinMaterial->SetVectorParameterValue(TEXT("Base Color"), FLinearColor(1.0f, 0.55f, 0.02f));
		CoinMaterial->SetScalarParameterValue(TEXT("Roughness"), 0.25f);
		ItemMesh->SetMaterial(0, CoinMaterial);
	}
}

void ACoinItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaSeconds, 0.0f));
}

void ACoinItem::ActiveItem(AActor* Activator)
{
	if (bCollected || !HasAuthority())
	{
		return;
	}

	AEX3_1Character* Character = Cast<AEX3_1Character>(Activator);
	if (!Character)
	{
		return;
	}

	bCollected = true;
	SetActorEnableCollision(false);
	Character->AddCoins(PointValue);

	AEX3_1GameMode* GameMode = GetWorld()->GetAuthGameMode<AEX3_1GameMode>();
	Destroy();

	if (GameMode)
	{
		GameMode->HandleCoinCollected();
	}
}
