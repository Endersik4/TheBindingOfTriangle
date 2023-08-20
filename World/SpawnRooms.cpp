// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/SpawnRooms.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "TheBindingOfTriangle/World/Door.h"


// Sets default values
ASpawnRooms::ASpawnRooms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FloorInstanceStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Floor Instance Static Mesh"));
	RootComponent = FloorInstanceStaticMesh;

	WallInstanceStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Wall Instance Static Mesh"));
	WallInstanceStaticMesh->SetupAttachment(FloorInstanceStaticMesh);

	ActivateRoomBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Activate Room Box Component"));
	ActivateRoomBoxComp->SetupAttachment(FloorInstanceStaticMesh);

	CameraLocationBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera Location Box Component"));
	CameraLocationBox->SetupAttachment(FloorInstanceStaticMesh);
	CameraLocationBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ASpawnRooms::BeginPlay()
{
	Super::BeginPlay();
	
	TrianglePawn = Cast<ATrianglePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (bShouldSpawn == true)
	{
		AmountOfSpawnedRooms = 0;
		bShouldSpawn = false;
	}
	InvertFirstDoorSide();
	SetUpRoom();

	ActivateRoomBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASpawnRooms::OnBoxBeginOverlap);
	ActivateRoomBoxComp->OnComponentEndOverlap.AddDynamic(this, &ASpawnRooms::OnBoxEndOverlap);
}

int ASpawnRooms::AmountOfSpawnedRooms = 0;

// Called every frame
void ASpawnRooms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SmoothCameraLocation(DeltaTime);
}

#pragma region ///////////////// SPAWN ROOM ///////////////////

void ASpawnRooms::SetUpRoom()
{
	FloorInstanceStaticMesh->ClearInstances();
	WallInstanceStaticMesh->ClearInstances();

	float x = 0.f;

	for (int32 i = 1; i <= NumberOfFloors_X; i++)
	{
		float y = 0.f;
			
		// Wall on the left
		AddWallAndDoor(i == NumberOfFloors_X - (NumberOfFloors_X / 2), EWS_Left, FVector(x, 0.f, 0.f));

		for (int32 j = 1; j <= NumberOfFloors_Y; j++)
		{
			FloorInstanceStaticMesh->AddInstance(FTransform(FVector(x, y, 0.f)));

			// Wall at the bottom
			if (i == 1)
			{
				AddWallAndDoor(j == NumberOfFloors_Y - (NumberOfFloors_Y / 2), EWS_Back, FVector(x, y, 0.f), FRotator(0.f, -90.f, 0.f));
			}

			// Wall at the top
			if (i == NumberOfFloors_X)
			{
				AddWallAndDoor(j == NumberOfFloors_Y - (NumberOfFloors_Y / 2), EWS_Front, FVector(x, y, 0.f), FRotator(0.f, 90.f, 0.f));
			}

			y += DistanceBetweenFloors_Y;
		}

		// Wall on the right
		AddWallAndDoor(i == NumberOfFloors_X - (NumberOfFloors_X / 2), EWS_Right, FVector(x, y - DistanceBetweenFloors_Y, 0.f), FRotator(0.f, 180.f,0.f));
		x += DistanceBetweenFloors_X;
	}
}

void ASpawnRooms::AddWallAndDoor(bool bAddDoor, EWallSide Side, FVector RelativeLocation, FRotator RelativeRotation)
{
	if (bAddDoor == true)
	{
		if (FirstDoorSide == Side)
		{
			GetWorld()->SpawnActor<ADoor>(DoorClass, GetActorLocation() + RelativeLocation, RelativeRotation);
			return;
		}

		bool bCanSpawnRoom = UKismetMathLibrary::RandomBoolWithWeight(0.5f);
		if (bCanSpawnRoom == true && AmountOfSpawnedRooms <= MaxRooms)
		{
			GetWorld()->SpawnActor<ADoor>(DoorClass, GetActorLocation() + RelativeLocation, RelativeRotation);
			AmountOfSpawnedRooms++;
			SpawnNewRoom(Side);
			return;
		}	
	}

	WallInstanceStaticMesh->AddInstance(FTransform(RelativeRotation, RelativeLocation));
}

void ASpawnRooms::SpawnNewRoom(EWallSide Side)
{
	FVector Location = GetActorLocation();
	switch (Side)
	{
	case EWS_Front: Location += GetActorForwardVector() * (DistanceBetweenFloors_X * NumberOfFloors_X + DistanceBetweenFloors_X); break;
	case EWS_Back: Location += -GetActorForwardVector() * (DistanceBetweenFloors_X * NumberOfFloors_X + DistanceBetweenFloors_X); break;
	case EWS_Right: Location += GetActorRightVector() * (DistanceBetweenFloors_Y * NumberOfFloors_Y + DistanceBetweenFloors_Y); break;
	case EWS_Left: Location += -GetActorRightVector() * (DistanceBetweenFloors_Y * NumberOfFloors_Y + DistanceBetweenFloors_Y); break;
	}
	FTransform SpawnLocAndRot = FTransform(FRotator(0.f), Location, FVector(1.f));
	ASpawnRooms* SpawnedRoom = GetWorld()->SpawnActorDeferred<ASpawnRooms>(NormalRoomClass, SpawnLocAndRot);
	SpawnedRoom->SetFirstDoorSide(Side);
	SpawnedRoom->FinishSpawning(SpawnLocAndRot);
}

void ASpawnRooms::InvertFirstDoorSide()
{
	if (FirstDoorSide == EWS_Front) FirstDoorSide = EWS_Back;
	else if (FirstDoorSide == EWS_Back) FirstDoorSide = EWS_Front;
	else if (FirstDoorSide == EWS_Right) FirstDoorSide = EWS_Left;
	else if (FirstDoorSide == EWS_Left) FirstDoorSide = EWS_Right;

}

#pragma endregion

#pragma region ///////////////// ACTIVATE ROOM BOX ///////////////////

void ASpawnRooms::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CameraStartPosition = TrianglePawn->TriangleCamera->GetActorLocation();
	bChangeCameraLocation = true;
}

void ASpawnRooms::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("OFF"));
}

void ASpawnRooms::SmoothCameraLocation(float Delta)
{
	if (bChangeCameraLocation == false) return;

	CameraLocationTimeElapsed += Delta;
	float t = FMath::Clamp(CameraLocationTimeElapsed / CameraChangeLocationTime, 0, 1);

	t = easeInOutCubic(t);

	FVector NewLocation = FMath::Lerp(CameraStartPosition, CameraLocationBox->GetComponentLocation(), t);
	TrianglePawn->TriangleCamera->SetActorLocation(NewLocation);

	if (t >= 1.f)
	{
		bChangeCameraLocation = false;
		CameraLocationTimeElapsed = 0.f;
	}
}

float ASpawnRooms::easeInOutCubic(float t)
{
	return t < 0.5 ? 4 * FMath::Pow(t, 3) : 1 - FMath::Pow(-2 * t + 2, 3) / 2;
}

#pragma endregion