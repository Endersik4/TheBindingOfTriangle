// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheBindingOfTriangle/Interfaces/TakeItemInterface.h"
#include "PickableItem.generated.h"

UENUM(BlueprintType)
enum TypeOfItem
{
	ECoin,
	EBomb,
	EKey,
	EHeart,
	ENone
};

UCLASS()
class THEBINDINGOFTRIANGLE_API APickableItem : public AActor, public ITakeItemInterface
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this actor's properties
	APickableItem();

	virtual void TakeItem(class ATrianglePawn* TrianglePawn) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCanPlayerBuyItem(bool bCan) { bCanPlayerBuyItem = bCan; }
	void ChangeIsSimulatingPhysics(bool bIs);
	int32 GetPriceForItem() const { return PriceForItem; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* ItemBoxComp;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		TEnumAsByte<TypeOfItem> ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Settings")
		int32 ItemAmount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings", meta = (EditCondition = "ItemType == TypeOfItem::EHeart", EditConditionHides))
		FString HeartName;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings", meta = (EditCondition = "ItemType == TypeOfItem::ECoin", EditConditionHides))
		TArray<int32> RandomCoinRange = { 1, 6 };

	UPROPERTY(EditAnywhere, Category = "Item Settings")
		float PopUpTime = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Item Settings")
		bool bCanPlayerBuyItem = false;
	UPROPERTY(EditAnywhere, Category = "Item Settings")
		int32 PriceForItem = 0;
private:

	// Change mesh scale from 0.f to original scale in Time (TimeEffect)
	bool bShouldMeshPopUp = true;
	float OriginalScale;
	float PopUpTimeElapsed;
	void PopUpMesh(float Delta);
};
