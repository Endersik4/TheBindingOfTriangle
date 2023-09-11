// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/GenerateLevel.h"
#include "Kismet/KismetMathLibrary.h"

#include "TheBindingOfTriangle/World/Room.h"
#include "TheBindingOfTriangle/TrianglePawnClasses/MinimapActor.h"

// Sets default values
AGenerateLevel::AGenerateLevel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGenerateLevel::BeginPlay()
{
	Super::BeginPlay();

	bDrawDebugRoomLayout = false;
	GenerateRoomsLayout();

	SpawnSpecificRoom(ERoomType::ERT_Award, EDoorType::EDT_Award);
	SpawnSpecificRoom(ERoomType::ERT_Shop, EDoorType::EDT_Shop);

	SpawnRooms();
	SetMinimapVariables();
}

// Called every frame
void AGenerateLevel::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);

}

#pragma region ////////////////// GENERATE ROOMS LAYOUT //////////////////
void AGenerateLevel::GenerateRoomsLayout()
{
	// Prevent the bug that cant find room neighbours because there is very small diffrence between room locations
	FVector TruncateLocation = FVector(FMath::TruncToInt(GetActorLocation().X), FMath::TruncToInt(GetActorLocation().Y), FMath::TruncToInt(GetActorLocation().Z));
	SetActorLocation(TruncateLocation);

	AllRoomsData.Empty();
	EndRoomsLocations.Empty();
	SpawnRoomsSeed.Reset();
	if (bDrawDebugRoomLayout == false) SpawnRoomsSeed.GenerateNewSeed();

	SpawnRoomLocation = GetActorLocation();
	AllRoomsData.Add(SpawnRoomLocation, FRoomStruct(SpawnRoomLocation, ERT_Spawn));

	if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), SpawnRoomLocation, FVector(200.f, 200.f, 0.f), FColor::Blue, true);

	for (int32 i = 0; i != HowManyRoutes; i++)
	{
		FVector NextRoomLocation = SpawnRoomLocation;
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
	if (bDrawDebugRoomLayout)
	{
		SpawnSpecificRoom(ERoomType::ERT_Award, EDoorType::EDT_Award);
		SpawnSpecificRoom(ERoomType::ERT_Shop, EDoorType::EDT_Shop);
	}
}

bool AGenerateLevel::CanRoomBeAtGivenLoc(FVector& RoomLocation, int32& Index, int32& StartRoomDistance, int32& SpawnRoomCounter)
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

bool AGenerateLevel::CheckNeighbours(const FVector& CenterRoomLoc, bool bAddDoors, FRoomStruct* CenterRoom, bool bChangeTypeNeighboursDoor)
{
	int32 Neighbours = 0;

	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + GetActorForwardVector() * DistanceBetweenRooms_X, 0, CenterRoom, bAddDoors, bChangeTypeNeighboursDoor);
	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + -GetActorForwardVector() * DistanceBetweenRooms_X, 1, CenterRoom, bAddDoors, bChangeTypeNeighboursDoor);
	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + GetActorRightVector() * DistanceBetweenRooms_Y,  2, CenterRoom, bAddDoors, bChangeTypeNeighboursDoor);
	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + -GetActorRightVector() * DistanceBetweenRooms_Y, 3, CenterRoom, bAddDoors, bChangeTypeNeighboursDoor);

	if (bChangeTypeNeighboursDoor) return true;

	if (bAddDoors)
	{
		CenterRoom->AmountOfDoors = Neighbours;
		return true; // There is no need to do the rest of the function
	}

	// Corners
	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + (GetActorForwardVector() * DistanceBetweenRooms_X + GetActorRightVector() * DistanceBetweenRooms_Y));
	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + (GetActorForwardVector() * DistanceBetweenRooms_X + -GetActorRightVector() * DistanceBetweenRooms_Y));
	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + (-GetActorForwardVector() * DistanceBetweenRooms_X + GetActorRightVector() * DistanceBetweenRooms_Y));
	Neighbours += WasNeighbourRoomFound(CenterRoomLoc + (-GetActorForwardVector() * DistanceBetweenRooms_X + -GetActorRightVector() * DistanceBetweenRooms_Y));

	return Neighbours >= 3;
}

int32 AGenerateLevel::WasNeighbourRoomFound(const FVector& DirToLookForNeighbour, int32 CenterRoomDoorSide, 
	FRoomStruct* CenterRoom, bool bAddDoors, bool bChangeTypeNeighboursDoor)
{
	FRoomStruct* FoundRoom = AllRoomsData.Find(DirToLookForNeighbour);
	if (FoundRoom == nullptr) return 0;

	if (bAddDoors) CenterRoom->DoorsType[CenterRoomDoorSide] = EDT_Room;
	if (bChangeTypeNeighboursDoor)
	{
		FoundRoom->DoorsType[GetDoorOnOppositeSide(CenterRoomDoorSide)] = CenterRoom->DoorsType[CenterRoomDoorSide];
	}

	return 1;
}

int32 AGenerateLevel::GetDoorOnOppositeSide(int32 CenterRoomDoorIndex)
{
	if (CenterRoomDoorIndex == 0) return 1;
	else if (CenterRoomDoorIndex == 1) return 0;
	else if (CenterRoomDoorIndex == 2) return 3;
	else return 2;
}

void AGenerateLevel::FinishRoomLayout()
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

void AGenerateLevel::SetEndRooms(FRoomStruct* FoundRoom)
{
	if (FoundRoom->AmountOfDoors != 1 || FoundRoom->RoomType == ERT_Spawn) return;

	FVector BossRoomLocation = FindFurthestRoom();

	if (FoundRoom->Location == BossRoomLocation)
	{
		FoundRoom->RoomType = ERT_Boss;
		for (int i = 0; i != 4; i++) if (FoundRoom->DoorsType[i] != EDT_None) FoundRoom->DoorsType[i] = EDT_Boss;
		CheckNeighbours(FoundRoom->Location, false, FoundRoom, true);
	}
	else
	{
		FoundRoom->RoomType = ERT_EndRoom;
		EndRoomsLocations.Add(FoundRoom->Location);
	}

	if (bDrawDebugRoomLayout == false) return;

	FColor NewColor = FoundRoom->RoomType == ERT_Boss ? FColor::Red : FColor::Emerald;
	DrawDebugBox(GetWorld(), FoundRoom->Location + FVector(0, 0, 1.f), FVector(200.f, 200.f, 0.f), NewColor, true);
}
#pragma endregion

#pragma region ///////////////// SPAWN ROOMS FROM LAYOUT //////////////////

void AGenerateLevel::SpawnRooms()
{
	TArray<FVector> RoomLocations;
	AllRoomsData.GenerateKeyArray(RoomLocations);

	for (const FVector& CurrRoomLoc : RoomLocations)
	{
		FRoomStruct* FoundRoom = AllRoomsData.Find(CurrRoomLoc);
		if (FoundRoom == nullptr) continue;

		TSubclassOf<ARoom> RoomClass = NormalRoomClass;
		if (FoundRoom->RoomType == ERT_Boss) RoomClass = BossRoomClass;
		else if (FoundRoom->RoomType == ERT_Award) RoomClass = AwardRoomClass;
		else if (FoundRoom->RoomType == ERT_Shop) RoomClass = ShopRoomClass;

		FTransform RoomTransfrom = FTransform(FRotator(0.f), CurrRoomLoc);
		ARoom* SpawnedRoom = GetWorld()->SpawnActorDeferred<ARoom>(RoomClass, RoomTransfrom);
		if (SpawnedRoom == nullptr) continue;

		SpawnedRoom->SetRoomData(*FoundRoom);
		SpawnedRoom->SetMinimapActor(MinimapActor);
		SpawnedRoom->FinishSpawning(RoomTransfrom);
	}
}

void AGenerateLevel::SpawnSpecificRoom(ERoomType RoomTypeToChoose, EDoorType DoorTypeToChoose, FColor DebugColor)
{
	FRoomStruct* SpecificRoom = PickRandomRoom(EndRoomsLocations);
	if (SpecificRoom == nullptr) return;

	SpecificRoom->RoomType = RoomTypeToChoose;
	EndRoomsLocations.Remove(SpecificRoom->Location);
	for (int i = 0; i != 4; i++)
	{
		if (SpecificRoom->DoorsType[i] == EDT_Room) SpecificRoom->DoorsType[i] = DoorTypeToChoose;
	}
	CheckNeighbours(SpecificRoom->Location, false, SpecificRoom, true);

	if (bDrawDebugRoomLayout == true) DrawDebugBox(GetWorld(), SpecificRoom->Location + FVector(0, 0, 2.f), FVector(200.f, 200.f, 0.f), DebugColor, true);
}

FVector AGenerateLevel::FindFurthestRoom()
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

void AGenerateLevel::SetMinimapVariables()
{
	if (MinimapActor == nullptr) return;

	MinimapActor->SetAllRoomsData(AllRoomsData);
	MinimapActor->SetDistanceBetweenRooms(FVector2D(DistanceBetweenRooms_X, DistanceBetweenRooms_Y));
	MinimapActor->PlayerHasMovedToNextRoom(SpawnRoomLocation);
}

FRoomStruct* AGenerateLevel::PickRandomRoom(TArray<FVector> RoomsLocations)
{
	if (RoomsLocations.Num() == 0) return nullptr;

	int32 RandRoom = UKismetMathLibrary::RandomIntegerInRangeFromStream(0, RoomsLocations.Num() - 1, SpawnRoomsSeed);
	return AllRoomsData.Find(RoomsLocations[RandRoom]);
}
