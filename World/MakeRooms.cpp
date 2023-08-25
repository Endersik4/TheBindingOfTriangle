// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/MakeRooms.h"
#include "Kismet/KismetMathLibrary.h"

#include "TheBindingOfTriangle/World/Room.h"

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

	GenerateRoomsLayout();
	SpawnAwardRoom();
	SpawnRooms();
}

// Called every frame
void AMakeRooms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region ////////////////// GENERATE ROOMS LAYOUT //////////////////
void AMakeRooms::GenerateRoomsLayout()
{	
	// Prevent the bug that cant find room neighbours because there is very small diffrence between room locations
	FVector TruncateLocation = FVector(FMath::TruncToInt(GetActorLocation().X), FMath::TruncToInt(GetActorLocation().Y), FMath::TruncToInt(GetActorLocation().Z));
	SetActorLocation(TruncateLocation);

	AllRoomsData.Empty();
	EndRoomsLocations.Empty();
	SpawnRoomsSeed.Reset();
	if (bDrawDebugRoomLayout == false) SpawnRoomsSeed.GenerateNewSeed();

	AllRoomsData.Add(GetActorLocation(), FRoomStruct(GetActorLocation(), ERT_Spawn));

	if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), GetActorLocation(), FVector(200.f, 200.f, 0.f), FColor::Blue, true);
	
	for (int32 i = 0; i != HowManyRoutes; i++)
	{
		FVector NextRoomLocation = GetActorLocation();
		int32 CantRoomSpawnCounter = 0;
		int32 DistanceFromStartRoom = 0; // In Rooms (1 - one room, 4 - four rooms...)

		for (int32 j = 0; j != HowManyRooms; j++)
		{
			int32 Side = UKismetMathLibrary::RandomIntegerInRangeFromStream(1, 4, SpawnRoomsSeed); // 1 - Forward, 2 - Back, 3 - Right, 4 - Left 

			switch (Side)
			{
			case 1: NextRoomLocation += GetActorForwardVector() * DistanceBetweenRooms_X; break;
			case 2: NextRoomLocation += -GetActorForwardVector() * DistanceBetweenRooms_X; break;
			case 3: NextRoomLocation += GetActorRightVector() * DistanceBetweenRooms_Y; break;
			case 4: NextRoomLocation += -GetActorRightVector() * DistanceBetweenRooms_Y; break;
			}

			if (CanRoomBeAtGivenLoc(NextRoomLocation, j, DistanceFromStartRoom, CantRoomSpawnCounter) == false) continue;

			DistanceFromStartRoom++;
			FRoomStruct NewRoom(NextRoomLocation, ERT_Normal, DistanceFromStartRoom);
			AllRoomsData.Add(NextRoomLocation, NewRoom);

			if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), NextRoomLocation, FVector(200.f, 200.f, 0.f), FColor::White, true);
		}
	}

	FinishRoomLayout();
}

bool AMakeRooms::CanRoomBeAtGivenLoc(FVector& RoomLocation, int32& Index, int32& StartRoomDistance, int32& SpawnRoomCounter)
{
	bool bTooManyNeighbours = CheckNeighbours(RoomLocation);

	if (AllRoomsData.Find(RoomLocation) != nullptr || bTooManyNeighbours == true)
	{
		SpawnRoomCounter++;
		if (SpawnRoomCounter > 100) Index = HowManyRooms; // Avoid the endless loop

		Index--;

		TArray<FVector> AllRoomsLocations;
		AllRoomsData.GenerateKeyArray(AllRoomsLocations);
		FRoomStruct* RandomRoom = PickRandomRoom(AllRoomsLocations);
		if (RandomRoom == nullptr) return false;

		StartRoomDistance = RandomRoom->DistanceFromStartRoom;
		RoomLocation = RandomRoom->Location;
		return false;
	}

	return true;
}

bool AMakeRooms::CheckNeighbours(const FVector& GridLoc, bool bAddDoors, FRoomStruct* GridRoom)
{
	int32 Neighbours = 0;
	if (AllRoomsData.Find(GridLoc + GetActorForwardVector() * DistanceBetweenRooms_X)) {
		Neighbours++;
		if (bAddDoors) GridRoom->DoorsType[0] = EDT_Room;
	}
	if (AllRoomsData.Find(GridLoc + -GetActorForwardVector() * DistanceBetweenRooms_X)) {
		Neighbours++;
		if (bAddDoors) GridRoom->DoorsType[1] = EDT_Room;
	}
	if (AllRoomsData.Find(GridLoc + GetActorRightVector() * DistanceBetweenRooms_Y)) {
		Neighbours++;
		if (bAddDoors)  GridRoom->DoorsType[2] = EDT_Room;
	}
	if (AllRoomsData.Find(GridLoc + -GetActorRightVector() * DistanceBetweenRooms_Y)) {
		Neighbours++;
		if (bAddDoors) GridRoom->DoorsType[3] = EDT_Room;
	}

	if (bAddDoors)
	{
		GridRoom->AmountOfDoors = Neighbours;
		return true; // There is no need to do the rest of the function
	}

	// Corners
	if (AllRoomsData.Find(GridLoc + (GetActorForwardVector() * DistanceBetweenRooms_X + GetActorRightVector() * DistanceBetweenRooms_Y))) Neighbours++;
	if (AllRoomsData.Find(GridLoc + (GetActorForwardVector() * DistanceBetweenRooms_X + -GetActorRightVector() * DistanceBetweenRooms_Y))) Neighbours++;
	if (AllRoomsData.Find(GridLoc + (-GetActorForwardVector() * DistanceBetweenRooms_X + GetActorRightVector() * DistanceBetweenRooms_Y))) Neighbours++;
	if (AllRoomsData.Find(GridLoc + (-GetActorForwardVector() * DistanceBetweenRooms_X + -GetActorRightVector() * DistanceBetweenRooms_Y))) Neighbours++;

	return Neighbours >= 3;
}

void AMakeRooms::FinishRoomLayout()
{
	TArray<FVector> RoomLocations;
	AllRoomsData.GenerateKeyArray(RoomLocations);

	for (const FVector& CurrRoomLoc : RoomLocations)
	{
		FRoomStruct* FoundRoom = AllRoomsData.Find(CurrRoomLoc);
		if (FoundRoom == nullptr) continue;

		// Make sure the door is properly positioned
		CheckNeighbours(CurrRoomLoc, true, FoundRoom);

		if (bDrawDebugRoomLayout == true)
		{
			if (FoundRoom->DoorsType[0] != EDT_None) DrawDebugBox(GetWorld(), CurrRoomLoc + GetActorForwardVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
			if (FoundRoom->DoorsType[1] != EDT_None) DrawDebugBox(GetWorld(), CurrRoomLoc + -GetActorForwardVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
			if (FoundRoom->DoorsType[2] != EDT_None) DrawDebugBox(GetWorld(), CurrRoomLoc + GetActorRightVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
			if (FoundRoom->DoorsType[3] != EDT_None) DrawDebugBox(GetWorld(), CurrRoomLoc + -GetActorRightVector() * 210.f, FVector(10.f, 10.f, 0.f), FColor::Emerald, true);
		}

		SetEndRooms(FoundRoom);
	}
}
#pragma endregion

#pragma region ///////////////// SPAWN ROOMS FROM LAYOUT //////////////////

void AMakeRooms::SpawnRooms()
{
	TArray<FVector> RoomLocations;
	AllRoomsData.GenerateKeyArray(RoomLocations);

	for (const FVector &CurrRoomLoc : RoomLocations)
	{
		FRoomStruct* FoundRoom = AllRoomsData.Find(CurrRoomLoc);
		if (FoundRoom == nullptr) continue;

		FTransform RoomTransfrom = FTransform(FRotator(0.f), CurrRoomLoc);
		ARoom* SpawnedRoom = GetWorld()->SpawnActorDeferred<ARoom>(NormalRoomClass, RoomTransfrom);
		if (SpawnedRoom == nullptr) continue;

		SpawnedRoom->SetRoomData(*FoundRoom);
		SpawnedRoom->FinishSpawning(RoomTransfrom);
	}
}

void AMakeRooms::SetEndRooms(FRoomStruct* FoundRoom)
{
	if (FoundRoom->AmountOfDoors == 1 && FoundRoom->RoomType != ERT_Spawn)
	{
		FVector BossRoomLocation = FindFurthestRoom();

		if (FoundRoom->Location == BossRoomLocation)
		{
			FoundRoom->RoomType = ERT_Boss;
			for (int i = 0; i != 4; i++) if (FoundRoom->DoorsType[i] != EDT_None) FoundRoom->DoorsType[i] = EDT_Boss;
		}
		else
		{
			FoundRoom->RoomType = ERT_EndRoom;
			EndRoomsLocations.Add(FoundRoom->Location);
		}

		if (bDrawDebugRoomLayout == true)
		{
			FColor NewColor = FoundRoom->RoomType == ERT_Boss ? FColor::Red : FColor::Emerald;
			DrawDebugBox(GetWorld(), FoundRoom->Location + FVector(0, 0, 1.f), FVector(200.f, 200.f, 0.f), NewColor, true);
		}
	}
}

void AMakeRooms::SpawnAwardRoom()
{
	FRoomStruct* AwardRoom = PickRandomRoom(EndRoomsLocations);
	if (AwardRoom == nullptr) return;

	AwardRoom->RoomType = ERT_Award;
	for (int i = 0; i != 4; i++) if (AwardRoom->DoorsType[i] == EDT_Room) AwardRoom->DoorsType[i] = EDT_Award;
	if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), AwardRoom->Location + FVector(0, 0, 2.f), FVector(200.f, 200.f, 0.f), FColor::Yellow, true);
}

FVector AMakeRooms::FindFurthestRoom()
{
	TArray<FRoomStruct> RoomGrids;
	AllRoomsData.GenerateValueArray(RoomGrids);
	FVector RoomLocation;
	int32 BiggestNumber = 0;

	for (const FRoomStruct& Room : RoomGrids)
	{
		if (Room.DistanceFromStartRoom < BiggestNumber) continue;
		
		BiggestNumber = Room.DistanceFromStartRoom;
		RoomLocation = Room.Location;
	}

	return RoomLocation;
}

#pragma endregion

FRoomStruct* AMakeRooms::PickRandomRoom(TArray<FVector> RoomsLocations)
{
	int32 RandRoom = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, RoomsLocations.Num() - 1, SpawnRoomsSeed);
	FRoomStruct* RandGrid = AllRoomsData.Find(RoomsLocations[RandRoom]);

	return RandGrid;
}
