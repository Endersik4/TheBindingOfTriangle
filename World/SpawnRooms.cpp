// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/SpawnRooms.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawnRooms::ASpawnRooms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FloorInstanceStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor Instance Static Mesh"));
	FloorInstanceStaticMesh->SetupAttachment(RootComponent);

	WallInstanceStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Wall Instance Static Mesh"));
	WallInstanceStaticMesh->SetupAttachment(FloorInstanceStaticMesh);
}

// Called when the game starts or when spawned
void ASpawnRooms::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnRooms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnRooms::SetUpRoom()
{
	FloorInstanceStaticMesh->ClearInstances();
	WallInstanceStaticMesh->ClearInstances();

	float x = 0.f;
	AmountOfWalls = 0;
	
	SetUpDoor();

	for (int32 i = 0; i != NumberOfFloors_X; i++)
	{
		float y = 0.f;
			
		// Wall on the left
		AddWallAndDoor(i % 2 == 1, FVector(x, 0.f, 0.f));

		for (int32 j = 0; j != NumberOfFloors_Y; j++)
		{
			FloorInstanceStaticMesh->AddInstance(FTransform(FVector(x, y, 0.f)));

			// Wall at the bottom
			if (i == 0)
			{
				AddWallAndDoor(j % 2 == 1, FVector(x, y, 0.f), FRotator(0.f, 90.f, 0.f));
			}

			// Wall at the top
			if (i == NumberOfFloors_X - 1)
			{
				AddWallAndDoor(j % 2 == 1, FVector(x + DistanceBetweenFloors_X, y, 0.f), FRotator(0.f, 90.f, 0.f));
			}

			y += DistanceBetweenFloors_Y;
		}

		// Wall on the right
		AddWallAndDoor(i % 2 == 1, FVector(x, y, 0.f));
		x += DistanceBetweenFloors_X;
	}
}

void ASpawnRooms::AddWallAndDoor(bool bCanSpawnDoor, FVector RelativeLocation, FRotator RelativeRotation)
{
	AmountOfWalls++;

	if (Doors.Num() > 0)
	{
		int32 IndexDoor = Doors.Find(AmountOfWalls);
		if (IndexDoor != INDEX_NONE)
		{
			Doors.RemoveAt(IndexDoor);
			NumberOfCurrentDoors.Add(1);
			
			return;
		}
	}

	WallInstanceStaticMesh->AddInstance(FTransform(RelativeRotation, RelativeLocation));
}

void ASpawnRooms::SetUpDoor()
{
	Doors.Empty();
	NumberOfCurrentDoors.Empty();

	int32 NumberOfWalls = 2 * (NumberOfFloors_X + NumberOfFloors_Y);
	if (NumberOfDoors > NumberOfWalls) NumberOfDoors = NumberOfWalls;

	for (int i = 0; i != NumberOfDoors; i++)
	{
		int32 RandVal = GetRandomValueForDoor(NumberOfWalls);
		Doors.Add(RandVal);
	}
}

int32 ASpawnRooms::GetRandomValueForDoor(int32 WallsAmount)
{
	int32 RandVal = UKismetMathLibrary::RandomIntegerInRangeFromStream(1, WallsAmount, RandomDoorSeed);

	if (Doors.Find(RandVal - 1) == INDEX_NONE && Doors.Find(RandVal + 1) == INDEX_NONE && Doors.Find(RandVal) == INDEX_NONE) return RandVal;
	else RandVal = GetRandomValueForDoor(WallsAmount);

	return RandVal;
}

