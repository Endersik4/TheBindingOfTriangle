// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Items/PickableItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"


// Sets default values
APickableItem::APickableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Item Box Component"));
	RootComponent = ItemBoxComp;
	ItemBoxComp->SetSimulatePhysics(true);
	ItemBoxComp->SetCollisionProfileName(FName(TEXT("ItemBox")));
	ItemBoxComp->GetBodyInstance()->bLockXRotation = true;
	ItemBoxComp->GetBodyInstance()->bLockYRotation = true;
	ItemBoxComp->GetBodyInstance()->bLockZRotation = true;
	ItemBoxComp->GetBodyInstance()->bLockZTranslation = true;
	ItemBoxComp->SetLinearDamping(6.f);

	ItemBoxComp->SetBoxExtent(FVector(32.f, 32.f, 100.f));

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetupAttachment(ItemBoxComp);
	ItemMesh->SetCollisionProfileName(FName(TEXT("ItemPreset")));

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void APickableItem::TakeItem(ATrianglePawn* TrianglePawn)
{
	if (bCanPlayerBuyItem == true)
	{
		int32 NewCoinsAmount = TrianglePawn->GetCoinsAmount() - PriceForItem;
		if (NewCoinsAmount < 0) return;
	}

	bool bWasItemAdded = false;
	if (ItemType == ECoin)
	{
		if (RandomCoinRange.Num() == 0 || RandomCoinRange.Num() == 1) return;

		int32 Amount = FMath::RandRange(RandomCoinRange[0], RandomCoinRange.Last());
		bWasItemAdded = TrianglePawn->AddCoins(Amount);
	}
	else if (ItemType == EBomb)
	{
		bWasItemAdded = TrianglePawn->AddBombs(ItemAmount);
	}
	else if (ItemType == EKey)
	{
		bWasItemAdded = TrianglePawn->AddKeys(ItemAmount);
	}
	else if (ItemType = EHeart)
	{
		bWasItemAdded = TrianglePawn->AddHearts(ItemAmount, HeartName);
	}

	if (bWasItemAdded == true && bCanPlayerBuyItem == true) TrianglePawn->AddCoins(-PriceForItem);
	if (bWasItemAdded == true) Destroy();
}

// Called when the game stats or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalScale = ItemMesh->GetRelativeScale3D().X;
	ItemMesh->SetRelativeScale3D(FVector(0.01f));
	if (ItemType == EHeart)
	{
		if (ItemAmount == 2) ItemMesh->SetMaterial(1, ItemMesh->GetMaterial(0));
	}
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldMeshPopUp == true) PopUpMesh(DeltaTime);
}


void APickableItem::PopUpMesh(float Delta)
{
	if (PopUpTimeElapsed <= PopUpTime)
	{
		float NewScale = FMath::Lerp(0.01f, OriginalScale, PopUpTimeElapsed / PopUpTime);
		ItemMesh->SetRelativeScale3D(FVector(NewScale));
		PopUpTimeElapsed += Delta;
	}
	else
	{
		ItemMesh->SetRelativeScale3D(FVector(OriginalScale));
		bShouldMeshPopUp = false;
	}
}

void APickableItem::ChangeIsSimulatingPhysics(bool bIs)
{
	ItemBoxComp->SetSimulatePhysics(bIs);
	ItemBoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
}
