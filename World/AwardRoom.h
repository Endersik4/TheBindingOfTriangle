// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheBindingOfTriangle/World/Room.h"
#include "AwardRoom.generated.h"

/**
 * 
 */
UCLASS()
class THEBINDINGOFTRIANGLE_API AAwardRoom : public ARoom
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAwardRoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* PedestalForItemMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Award Room Settings")
		TArray<TSubclassOf<class AAwardItem>> RandomAwardItemList;

	void PickRandomAwardFromList();
};
