#include "PotionItem.h"

#include "EX3_1Character.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

APotionItem::APotionItem()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemType = TEXT("Potion");
}

void APotionItem::BeginPlay()
{
	Super::BeginPlay();
	if (UMaterialInterface* BaseMaterial = ItemMesh->GetMaterial(0))
	{
		UMaterialInstanceDynamic* PotionMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		PotionMaterial->SetVectorParameterValue(TEXT("Base Color"), FLinearColor(0.02f, 0.8f, 0.15f));
		PotionMaterial->SetScalarParameterValue(TEXT("Roughness"), 0.2f);
		ItemMesh->SetMaterial(0, PotionMaterial);
	}
}

void APotionItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaSeconds, 0.0f));
}

void APotionItem::ActiveItem(AActor* Activator)
{
	if (bConsumed || !HasAuthority())
	{
		return;
	}

	AEX3_1Character* Character = Cast<AEX3_1Character>(Activator);
	if (!Character)
	{
		return;
	}

	bConsumed = true;
	SetActorEnableCollision(false);
	Character->Heal(HealAmount);
	Destroy();
}
