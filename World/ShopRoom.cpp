// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/ShopRoom.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"

#include "TheBindingOfTriangle/Items/PickableItem.h"

AShopRoom::AShopRoom()
{
	FirstItemLocationBox = CreateDefaultSubobject<UBoxComponent>(TEXT("First Item Location Box"));
	FirstItemLocationBox->SetupAttachment(RootComponent);
	SecondItemLocationBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Second Item Location Box"));
	SecondItemLocationBox->SetupAttachment(RootComponent);
	ThirdItemLocationBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Third Item Location Box"));
	ThirdItemLocationBox->SetupAttachment(RootComponent);

	FirstItemPriceText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("First Item Price Text"));
	FirstItemPriceText->SetupAttachment(RootComponent);
	SecondItemPriceText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Second Item Price Text"));
	SecondItemPriceText->SetupAttachment(RootComponent);
	ThirdItemPriceText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Third Item Price Text"));
	ThirdItemPriceText->SetupAttachment(RootComponent);
}

void AShopRoom::BeginPlay()
{
	Super::BeginPlay();

}

void AShopRoom::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBoxBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (bItemsSpawned == false)
	{
		FillItemsArray();
		SpawnItemsToBuy();
		bItemsSpawned = true;
	}
}

void AShopRoom::FillItemsArray()
{
	ItemsLocations.Add(FirstItemLocationBox);
	ItemsLocations.Add(SecondItemLocationBox);
	ItemsLocations.Add(ThirdItemLocationBox);

	ItemsPriceText.Add(FirstItemPriceText);
	ItemsPriceText.Add(SecondItemPriceText);
	ItemsPriceText.Add(ThirdItemPriceText);
}

void AShopRoom::SpawnItemsToBuy()
{
	for (int i = 0; i != 3; i++)
	{
		int32 RandomItem = FMath::RandRange(0, ListOfItemsToBuy.Num() - 1);

		FTransform NewTransform = FTransform(FRotator(0.f), ItemsLocations[i]->GetComponentLocation());
		APickableItem* SpawnedItem = GetWorld()->SpawnActorDeferred<APickableItem>(ListOfItemsToBuy[RandomItem], NewTransform);
		SpawnedItem->SetCanPlayerBuyItem(true);
		SpawnedItem->ChangeIsSimulatingPhysics(false);

		FText NewText = FText::FromString(FString::FromInt(SpawnedItem->GetPriceForItem()));
		ItemsPriceText[i]->SetText(NewText);

		SpawnedItem->FinishSpawning(NewTransform);
	}
}

