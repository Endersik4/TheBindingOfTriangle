// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomData.h"

#include "GenerateLevel.generated.h"

UCLASS()
class THEBINDINGOFTRIANGLE_API AGenerateLevel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGenerateLevel();

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
		class AMinimapActor* MinimapActor;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 HowManyRoutes = 5;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		int32 HowManyRooms = 3;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		float DistanceBetweenRooms_X = 1360.f;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		float DistanceBetweenRooms_Y = 2440.f;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		TSubclassOf<class ARoom> NormalRoomClass;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		TSubclassOf<class ARoom> AwardRoomClass;	
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		TSubclassOf<class ARoom> BossRoomClass;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		TSubclassOf<class ARoom> ShopRoomClass;
	UPROPERTY(EditAnywhere, Category = "Room Spawner settings")
		FRandomStream SpawnRoomsSeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Spawner settings", meta = (AllowPrivateAccess = "true"))
		bool bDrawDebugRoomLayout = false;

	UPROPERTY(VisibleAnywhere)
		TMap<FVector, FRoomStruct> AllRoomsData;

	FVector SpawnRoomLocation;

	TArray<FVector> EndRoomsLocations;

	bool CanRoomBeAtGivenLoc(FVector& RoomLocation, int32& Index, int32& StartRoomDistance, int32& SpawnRoomCounter);
	bool CheckNeighbours(const FVector& GridLoc, bool bAddDoors = false, FRoomStruct* GridRoom = nullptr, bool bChangeTypeNeighboursDoor = false);

	void FinishRoomLayout();
	void SpawnRooms();
	void SetEndRooms(FRoomStruct* FoundRoom);
	void SpawnSpecificRoom(ERoomType RoomTypeToChoose, EDoorType DoorTypeToChoose, FColor DebugColor = FColor::Yellow);
	FVector FindFurthestRoom();

	FRoomStruct* PickRandomRoom(TArray<FVector> RoomsLocations);

};
