// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"


ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(SceneRoot);
	CollisionSphere->SetSphereRadius(65.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetGenerateOverlapEvents(true);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneRoot);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnActorBeginOverlap.AddDynamic(this, &ABaseItem::HandleActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABaseItem::HandleActorEndOverlap);
}

void ABaseItem::HandleActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		OnItemOverlap(OtherActor);
	}
}

void ABaseItem::HandleActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != this)
	{
		OnItemEndOverlap(OtherActor);
	}
}

void ABaseItem::OnItemOverlap(AActor* OverlapActor) 
{
	if (const APawn* Pawn = Cast<APawn>(OverlapActor); Pawn && Pawn->IsPlayerControlled())
	{
		ActiveItem(OverlapActor);
	}
}

void ABaseItem::OnItemEndOverlap(AActor* OverlapActor) 
{
}

void ABaseItem::ActiveItem(AActor* Activator) 
{
}

FName ABaseItem::GetItemType() const 
{
	return ItemType;
}

