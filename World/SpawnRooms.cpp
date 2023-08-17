// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/SpawnRooms.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"


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

	ActivateRoomBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ASpawnRooms::OnBoxBeginOverlap);
	ActivateRoomBoxComp->OnComponentEndOverlap.AddDynamic(this, &ASpawnRooms::OnBoxEndOverlap);

	CameraLocationBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera Location Box Component"));
	CameraLocationBox->SetupAttachment(FloorInstanceStaticMesh);
	CameraLocationBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ASpawnRooms::BeginPlay()
{
	Super::BeginPlay();
	
	TrianglePawn = Cast<ATrianglePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

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
		AddWallAndDoor(i == NumberOfFloors_X - (NumberOfFloors_X / 2), FVector(x, 0.f, 0.f));

		for (int32 j = 1; j <= NumberOfFloors_Y; j++)
		{
			FloorInstanceStaticMesh->AddInstance(FTransform(FVector(x, y, 0.f)));

			// Wall at the bottom
			if (i == 1)
			{
				AddWallAndDoor(j == NumberOfFloors_Y - (NumberOfFloors_Y / 2), FVector(x, y, 0.f), FRotator(0.f, 90.f, 0.f));
			}

			// Wall at the top
			if (i == NumberOfFloors_X)
			{
				AddWallAndDoor(j == NumberOfFloors_Y - (NumberOfFloors_Y / 2), FVector(x + DistanceBetweenFloors_X, y, 0.f), FRotator(0.f, 90.f, 0.f));
			}

			y += DistanceBetweenFloors_Y;
		}

		// Wall on the right
		AddWallAndDoor(i == NumberOfFloors_X - (NumberOfFloors_X / 2), FVector(x, y, 0.f));
		x += DistanceBetweenFloors_X;
	}
}

void ASpawnRooms::AddWallAndDoor(bool bAddDoor, FVector RelativeLocation, FRotator RelativeRotation)
{
	if (bAddDoor == false) WallInstanceStaticMesh->AddInstance(FTransform(RelativeRotation, RelativeLocation));
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