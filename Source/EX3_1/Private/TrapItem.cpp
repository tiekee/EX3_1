#include "TrapItem.h"

#include "EX3_1Character.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"

ATrapItem::ATrapItem()
{
	ItemType = TEXT("Trap");
}

void ATrapItem::BeginPlay()
{
	Super::BeginPlay();
	if (UMaterialInterface* BaseMaterial = ItemMesh->GetMaterial(0))
	{
		UMaterialInstanceDynamic* TrapMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		TrapMaterial->SetVectorParameterValue(TEXT("Base Color"), FLinearColor(0.8f, 0.02f, 0.01f));
		TrapMaterial->SetScalarParameterValue(TEXT("Roughness"), 0.4f);
		ItemMesh->SetMaterial(0, TrapMaterial);
	}
}

void ATrapItem::OnItemOverlap(AActor* OverlapActor)
{
	if (bArmed || !HasAuthority())
	{
		return;
	}

	AEX3_1Character* Character = Cast<AEX3_1Character>(OverlapActor);
	if (!Character || !Character->IsPlayerControlled())
	{
		return;
	}

	bArmed = true;
	TriggeringCharacter = Character;
	GetWorldTimerManager().SetTimer(ActivationTimer, this, &ATrapItem::TriggerTrap, ActivationDelay, false);
}

void ATrapItem::TriggerTrap()
{
	AEX3_1Character* Character = TriggeringCharacter.Get();
	if (Character && CollisionSphere->IsOverlappingActor(Character))
	{
		UGameplayStatics::ApplyDamage(Character, DamageAmount, nullptr, this, nullptr);
	}

	SetActorEnableCollision(false);
	Destroy();
}
