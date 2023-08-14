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
	EKey
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


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		TEnumAsByte<TypeOfItem> ItemType;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings", meta = (EditCondition = "ItemType == TypeOfItem::ECoin", EditConditionHides))
		TArray<int32> RandomCoinRange = { 1, 6 };
};
