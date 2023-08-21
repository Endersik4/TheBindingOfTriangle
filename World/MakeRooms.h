// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MakeRooms.generated.h"

enum ERoomType {
	ERT_Normal,
	ERT_Award,
	ERT_Boss,
	ERT_EndRoom,
	ERT_Spawn
};


USTRUCT(BlueprintType)
struct FRoom {

	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		FVector Location = FVector(0.f);

	ERoomType RoomType = ERT_Normal;
	int32 DistanceFromStartRoom = 0;
	int32 AmountOfDoors = 0;
	bool bDoorTop, bDoorBot, bDoorRight, bDoorLeft;

	FRoom(FVector NewLoc = FVector(0.f), ERoomType Type = ERT_Normal, int32 Distance = 0, int32 Doors = 0) : Location(NewLoc), RoomType(Type), DistanceFromStartRoom(Distance), AmountOfDoors(Doors)
	{
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
		void GenerateRoomsLayout();

private:
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 HowManyRoutes = 3;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 HowManyRooms = 4;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		float DistanceBetweenRooms_X = 440.f;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		float DistanceBetweenRooms_Y = 440.f;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		FRandomStream SpawnRoomsSeed;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		bool bDrawDebugRoomLayout = false;

	TMap<FVector, FRoom> AllRoomsData;
	TMap<FVector, FRoom> EndRoomsData;

	bool CanRoomBeAtGivenLoc(FVector &RoomLocation, int32 &Index, int32& StartRoomDistance, int32& SpawnRoomCounter);
	FVector PickRandomRoomLocation(FRoom& PickedGrid, const TMap<FVector, FRoom>& RoomsData);

	bool CheckNeighbours(const FVector& GridLoc, bool bAddDoors = false, FRoom* GridRoom = nullptr);

	void SpawnRooms();
	FVector FindFurthestRoom();
	void SpawnAwardRoom();
};
