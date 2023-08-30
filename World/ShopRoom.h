// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheBindingOfTriangle/World/Room.h"
#include "ShopRoom.generated.h"

/**
 * 
 */
class UBoxComponent;
class UTextRenderComponent;

UCLASS()
class THEBINDINGOFTRIANGLE_API AShopRoom : public ARoom
{
	GENERATED_BODY()

public:
	AShopRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UBoxComponent* FirstItemLocationBox;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UBoxComponent* SecondItemLocationBox;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UBoxComponent* ThirdItemLocationBox;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UTextRenderComponent* FirstItemPriceText;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UTextRenderComponent* SecondItemPriceText;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UTextRenderComponent* ThirdItemPriceText;

	UPROPERTY(EditDefaultsOnly, Category = "Shop Settings")
		TArray<TSubclassOf<class APickableItem>>  ListOfItemsToBuy;

	void FillItemsArray();
	void SpawnItemsToBuy();

	TArray<UBoxComponent*> ItemsLocations;
	TArray<UTextRenderComponent*> ItemsPriceText;
};
