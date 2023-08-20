// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MakeRooms.generated.h"

USTRUCT(BlueprintType)
struct FGrid {

	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		FVector Location;
	UPROPERTY(EditAnywhere)
		bool bIsOccupied;

	FGrid()
	{
		Location = FVector(0.f);
		bIsOccupied = false;
	}

	FGrid(FVector NewLoc, bool bOccupied)
	{
		Location = NewLoc;
		bIsOccupied = bOccupied;
	}
};

UCLASS()
class THEBINDINGOFTRIANGLE_API AMakeRooms : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMakeRooms();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SpawnDebugGrid();

private:
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 FriendsAmount = 2;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 HowManyRooms = 8;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 Rows_Grid_X = 5;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 Rows_Grid_Y = 7;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		float DistanceBetweenGrid_X = 1440.f;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		float DistanceBetweenGrid_Y = 1440.f;

	TMap<FVector, FGrid> GridData;

};
