// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RoomData.generated.h"

UENUM(BlueprintType)
enum EDoorStatus {
	EDS_Open,
	EDS_Closed,
	EDS_KeyRequired
};

UENUM(BlueprintType)
enum EDoorType {
	EDT_Room,
	EDT_Boss,
	EDT_Award,
	EDT_None
};

UENUM(BlueprintType)
enum ERoomType {
	ERT_Normal,
	ERT_Award,
	ERT_Boss,
	ERT_EndRoom,
	ERT_Spawn
};

USTRUCT(BlueprintType)
struct FRoomStruct {

	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, Category = "Struct Variables")
		FVector Location = FVector(0.f);
	UPROPERTY(VisibleAnywhere, Category = "Struct Variables")
		TEnumAsByte<ERoomType> RoomType = ERT_Normal;
	UPROPERTY(EditAnywhere, Category = "Struct Variables")
		int32 DistanceFromStartRoom = 0;
	UPROPERTY(EditAnywhere, Category = "Struct Variables")
		int32 AmountOfDoors = 0;
	UPROPERTY(VisibleAnywhere, Category = "Struct Variables")
		TEnumAsByte<EDoorType> DoorsType[4] = { EDT_None, EDT_None, EDT_None, EDT_None }; // 1 - Top, 2 - Bottom 3 - Right, 4 - left

	FRoomStruct(FVector NewLoc = FVector(0.f), ERoomType Type = ERT_Normal, int32 Distance = 0, int32 Doors = 0) : Location(NewLoc), RoomType(Type), DistanceFromStartRoom(Distance), AmountOfDoors(Doors)
	{
	}
};

/**
 * 
 */
class THEBINDINGOFTRIANGLE_API RoomData
{

};
