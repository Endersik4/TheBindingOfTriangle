// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Items/PickableItem.h"
#include "Components/StaticMeshComponent.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"


// Sets default values
APickableItem::APickableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	RootComponent = ItemMesh;
	ItemMesh->SetCollisionProfileName(FName(TEXT("ECC_GameTraceChannel2")));
	ItemMesh->SetCollisionObjectType(ECC_GameTraceChannel2);

}

void APickableItem::TakeItem(ATrianglePawn* TrianglePawn)
{
	bool bWasItemAdded;
	if (ItemType == ECoin)
	{
		if (RandomCoinRange.Num() == 0 || RandomCoinRange.Num() == 1) return;

		int32 Amount = FMath::RandRange(RandomCoinRange[0], RandomCoinRange.Last());
		bWasItemAdded = TrianglePawn->AddCoins(Amount);
	}
	else if (ItemType == EBomb)
	{
		bWasItemAdded = TrianglePawn->AddBombs(1);
	}
	else if (ItemType == EKey)
	{
		bWasItemAdded = TrianglePawn->AddKeys(1);
	}

	if (bWasItemAdded == true) Destroy();
}

// Called when the game stats or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

