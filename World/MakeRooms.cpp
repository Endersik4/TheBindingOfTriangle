// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/MakeRooms.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMakeRooms::AMakeRooms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMakeRooms::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMakeRooms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region ////////////////// GENERATE ROOMS LAYOUT //////////////////
void AMakeRooms::GenerateRoomsLayout()
{	
	AllRoomsData.Empty();
	EndRoomsData.Empty();
	SpawnRoomsSeed.Reset();

	AllRoomsData.Add(GetActorLocation(), FRoom(GetActorLocation(), ERT_Spawn));

	if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), GetActorLocation(), FVector(200.f, 200.f, 0.f), FColor::Blue, true);
	
	for (int32 i = 0; i != HowManyRoutes; i++)
	{
		FVector NextRoomLocation = GetActorLocation();
		int32 CantRoomSpawnCounter = 0;
		int32 DistanceFromStartRoom = 0; // In Rooms (1 - one room, 4 - four rooms...)

		for (int32 j = 0; j != HowManyRooms; j++)
		{
			int32 Side = UKismetMathLibrary::RandomIntegerInRangeFromStream(1, 4, SpawnRoomsSeed); // 1 - Forward, 2 - Back, 3 - Right, 4 - Left 
			FVector NextRoomDirection;

			switch (Side)
			{
			case 1: NextRoomDirection = GetActorForwardVector(); break;
			case 2: NextRoomDirection = -GetActorForwardVector(); break;
			case 3: NextRoomDirection = GetActorRightVector(); break;
			case 4: NextRoomDirection = -GetActorRightVector(); break;
			}

			NextRoomLocation += NextRoomDirection * DistanceBetweenRooms_X;

			if (CanRoomBeAtGivenLoc(NextRoomLocation, j, DistanceFromStartRoom, CantRoomSpawnCounter) == false) continue;

			DistanceFromStartRoom++;
			FRoom NewRoom(NextRoomLocation, ERT_Normal, DistanceFromStartRoom);
			AllRoomsData.Add(NextRoomLocation, NewRoom);

			if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), NextRoomLocation, FVector(200.f, 200.f, 0.f), FColor::White, true);
		}
	}

	SpawnRooms();
	SpawnAwardRoom();
}

bool AMakeRooms::CanRoomBeAtGivenLoc(FVector& RoomLocation, int32& Index, int32& StartRoomDistance, int32& SpawnRoomCounter)
{
	bool bTooManyNeighbours = CheckNeighbours(RoomLocation);

	if (AllRoomsData.Find(RoomLocation) != nullptr || bTooManyNeighbours == true)
	{
		SpawnRoomCounter++;
		if (SpawnRoomCounter > 100) Index = HowManyRooms; // Avoid the endless loop

		Index--;
		FRoom Room;
		RoomLocation = PickRandomRoomLocation(Room, AllRoomsData);
		StartRoomDistance = Room.DistanceFromStartRoom;
		return false;
	}

	return true;
}

bool AMakeRooms::CheckNeighbours(const FVector& GridLoc, bool bAddDoors, FRoom* GridRoom)
{
	int32 Neighbours = 0;
	if (AllRoomsData.Find(GridLoc + GetActorForwardVector() * DistanceBetweenRooms_X)) {
		Neighbours++;
		if (bAddDoors) { GridRoom->bDoorTop = true; }
	}
	if (AllRoomsData.Find(GridLoc + -GetActorForwardVector() * DistanceBetweenRooms_X)) {
		Neighbours++;
		if (bAddDoors) GridRoom->bDoorBot = true;
	}
	if (AllRoomsData.Find(GridLoc + GetActorRightVector() * DistanceBetweenRooms_X)) {
		Neighbours++;
		if (bAddDoors) GridRoom->bDoorRight = true;
	}
	if (AllRoomsData.Find(GridLoc + -GetActorRightVector() * DistanceBetweenRooms_X)) {
		Neighbours++;
		if (bAddDoors) GridRoom->bDoorLeft = true;
	}
	if (bAddDoors)
	{
		GridRoom->AmountOfDoors = Neighbours;
		return true; // There is no need to perform the rest of the function
	}

	// Corners
	if (AllRoomsData.Find(GridLoc + (GetActorForwardVector() + GetActorRightVector()) * DistanceBetweenRooms_X)) Neighbours++;
	if (AllRoomsData.Find(GridLoc + (GetActorForwardVector() + -GetActorRightVector()) * DistanceBetweenRooms_X)) Neighbours++;
	if (AllRoomsData.Find(GridLoc + (-GetActorForwardVector() + GetActorRightVector()) * DistanceBetweenRooms_X)) Neighbours++;
	if (AllRoomsData.Find(GridLoc + (-GetActorForwardVector() + -GetActorRightVector()) * DistanceBetweenRooms_X)) Neighbours++;

	return Neighbours >= 3;
}

#pragma endregion

void AMakeRooms::SpawnRooms()
{
	TArray<FVector> RoomLocations;
	AllRoomsData.GenerateKeyArray(RoomLocations);
	FVector BossRoomLocation = FindFurthestRoom();

	for (const FVector &CurrLoc : RoomLocations)
	{
		FRoom* Grid = AllRoomsData.Find(CurrLoc);
		if (Grid == nullptr) continue;

		// Make sure the door is properly positioned
		CheckNeighbours(CurrLoc, true, Grid);

		if (bDrawDebugRoomLayout == true)
		{
			if (Grid->bDoorTop == true)	DrawDebugBox(GetWorld(), CurrLoc + GetActorForwardVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
			if (Grid->bDoorBot == true) DrawDebugBox(GetWorld(), CurrLoc + -GetActorForwardVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
			if (Grid->bDoorRight == true) DrawDebugBox(GetWorld(), CurrLoc + GetActorRightVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
			if (Grid->bDoorLeft == true) DrawDebugBox(GetWorld(), CurrLoc + -GetActorRightVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
		}

		if (Grid->AmountOfDoors == 1 && Grid->RoomType != ERT_Spawn)
		{
			FColor NewColor = FColor::Emerald;
			if (CurrLoc == BossRoomLocation)
			{
				NewColor = FColor::Red;
				Grid->RoomType = ERT_Boss;
			}
			else
			{
				Grid->RoomType = ERT_EndRoom;
				EndRoomsData.Add(CurrLoc, *Grid);
			}
			if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), CurrLoc + FVector(0, 0, 1.f), FVector(200.f, 200.f, 0.f), NewColor, true);
		}
		
	}
}

FVector AMakeRooms::FindFurthestRoom()
{
	TArray<FRoom> RoomGrids;
	AllRoomsData.GenerateValueArray(RoomGrids);
	FVector RoomLocation;
	int32 BiggestNumber = 0;

	for (const FRoom& Room : RoomGrids)
	{
		if (Room.DistanceFromStartRoom < BiggestNumber) continue;
		
		BiggestNumber = Room.DistanceFromStartRoom;
		RoomLocation = Room.Location;
	}

	return RoomLocation;
}

void AMakeRooms::SpawnAwardRoom()
{
	FRoom AwardRoom;
	FVector AwardLoc = PickRandomRoomLocation(AwardRoom, EndRoomsData);
	AwardRoom.RoomType = ERT_Award;
	DrawDebugBox(GetWorld(), AwardLoc + FVector(0,0,2.f), FVector(200.f, 200.f, 0.f), FColor::Yellow, true);
}

FVector AMakeRooms::PickRandomRoomLocation(FRoom& PickedGrid, const TMap<FVector, FRoom>& RoomsData)
{
	TArray<FVector> TempRoomsLocation;
	RoomsData.GenerateKeyArray(TempRoomsLocation);

	int32 RandRoom = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, TempRoomsLocation.Num() - 1, SpawnRoomsSeed);
	FRoom* RandGrid = AllRoomsData.Find(TempRoomsLocation[RandRoom]);

	if (RandGrid != nullptr) PickedGrid = *RandGrid;
	return TempRoomsLocation[RandRoom];
}
