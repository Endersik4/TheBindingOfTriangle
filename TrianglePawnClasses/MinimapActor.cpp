// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/MinimapActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/BoxComponent.h"
#include "PaperSpriteActor.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMinimapActor::AMinimapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxMainComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Main Component"));
	RootComponent = BoxMainComponent;
	BoxMainComponent->SetCollisionProfileName(FName(TEXT("NoCollision")));

	MinimapSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Minimap Scene Capture"));
	MinimapSceneCapture->SetupAttachment(BoxMainComponent);
	MinimapSceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
}

// Called when the game starts or when spawned
void AMinimapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMinimapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveCurrentMinimapRoom(DeltaTime);
}

void AMinimapActor::GetRoomNeighboursLocation(TArray<FVector>& RoomNeighboursLocation, FVector RealRoomLocation)
{
	RoomNeighboursLocation.Add(RealRoomLocation);

	// Neighbours of RealRoomLocation
	RoomNeighboursLocation.Add(RealRoomLocation + FVector(DistanceBetweenRooms.X, 0.f, 0.f));
	RoomNeighboursLocation.Add(RealRoomLocation + FVector(-DistanceBetweenRooms.X, 0.f, 0.f));
	RoomNeighboursLocation.Add(RealRoomLocation + FVector(0, DistanceBetweenRooms.Y, 0.f));
	RoomNeighboursLocation.Add(RealRoomLocation + FVector(0.f, -DistanceBetweenRooms.Y, 0.f));
}

void AMinimapActor::MakeRoomMinimapWithNeighbours(const FVector& RealRoomLocation)
{
	TArray<FVector> RoomNeighboursLocation;
	GetRoomNeighboursLocation(RoomNeighboursLocation, RealRoomLocation);

	for (int i = 0; i != 5; i++)
	{
		const FRoomStruct* CurrMinimapRoom = AllRoomsData.Find(RoomNeighboursLocation[i]);
		if (CurrMinimapRoom == nullptr) continue;

		// i == 0 == The room in which the player is located
		if (i == 0) NextRoomColor = GetProperRoomColor(CurrMinimapRoom->RoomType);

		FVector Location = TransformRoomLocation(CurrMinimapRoom->Location);
		if (AllMinimapRooms.Find(Location)) continue;

		SpawnRoomSprite(Location, CurrMinimapRoom, i == 0);
	}
}

FColor& AMinimapActor::GetProperRoomColor(const ERoomType& RoomType, bool bFirstTimeSpawned)
{
	if (RoomType == ERT_Award) return AwardRoomColor;
	else if (RoomType == ERT_Boss) return BossRoomColor;
	else if (RoomType == ERT_Shop) return ShopRoomColor;
	else if (bFirstTimeSpawned == true)
	{
		if (RoomType == ERT_Spawn) return CurrentRoomColor;
		else return NotDiscoveredRoomColor;
	}
	else return NormalRoomColor;
}

FVector AMinimapActor::TransformRoomLocation(const FVector& RealRoomLocation)
{
	FVector Location = RealRoomLocation;
	Location.X /= LocationDivider.X;
	Location.Y /= LocationDivider.Y;
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), Location);
}

void AMinimapActor::SpawnRoomSprite(const FVector & SpawnLocation, const FRoomStruct* RoomData, bool bFirstMinimapRoom)
{
	APaperSpriteActor* SpawnedSpriteActor = GetWorld()->SpawnActor<APaperSpriteActor>(APaperSpriteActor::StaticClass(), SpawnLocation, FRotator(0.f, 90.f, 90.f));
	if (SpawnedSpriteActor == nullptr) return;

	const FColor SpriteRoomColor = GetProperRoomColor(RoomData->RoomType, true);

	if (bFirstMinimapRoom) CurrentMinimapRoom = SpawnedSpriteActor;

	SpawnedSpriteActor->SetActorScale3D(FVector(0.1f));
	SpawnedSpriteActor->GetRenderComponent()->SetMobility(EComponentMobility::Movable); // There is a bug where Sprite cant be changed when Mobility != Movable
	SpawnedSpriteActor->GetRenderComponent()->SetSprite(RoomPaperSprite);
	SpawnedSpriteActor->GetRenderComponent()->SetSpriteColor(SpriteRoomColor);

	AllMinimapRooms.Add(SpawnLocation, SpawnedSpriteActor);
}

void AMinimapActor::PlayerHasMovedToNextRoom(FVector NextRoomLocation)
{
	MakeRoomMinimapWithNeighbours(NextRoomLocation);
	FVector Location = TransformRoomLocation(NextRoomLocation);

	APaperSpriteActor* FoundActor = *AllMinimapRooms.Find(Location);
	if (FoundActor == nullptr || FoundActor == CurrentMinimapRoom) return;

	MinimapNextRoom = FoundActor;
	MoveSceneCapture(true, Location);
}

void AMinimapActor::SetInitialRoomColor(const ERoomType& InitialRoomType)
{
	OriginalRoomColor = GetProperRoomColor(InitialRoomType);
}

#pragma region ////////////////// MOVE SCENE CAPTURE /////////////////////
void AMinimapActor::MoveSceneCapture(bool bChangeLoc, FVector CameraLocation)
{
	bChangeCurrentMinimapRoom = bChangeLoc;

	SceneCaptureStartPosition = MinimapSceneCapture->GetRelativeLocation();
	SceneCaptureEndPosition = CameraLocation;
	SceneCaptureEndPosition.Z = SceneCaptureStartPosition.Z;
}

void AMinimapActor::MoveCurrentMinimapRoom(float Delta)
{
	if (bChangeCurrentMinimapRoom == false) return;

	SceneCaptureLocationTimeElapsed += Delta;

	float t = FMath::Clamp(SceneCaptureLocationTimeElapsed / SceneCaptureChangeLocationTime, 0, 1);
	t = easeInOutCubic(t);

	FVector NewLocation = FMath::Lerp(SceneCaptureStartPosition, SceneCaptureEndPosition, t);
	MinimapSceneCapture->SetRelativeLocation(NewLocation);

	FLinearColor NewCurrRoomColor = UKismetMathLibrary::LinearColorLerp(CurrentRoomColor, OriginalRoomColor, t);
	CurrentMinimapRoom->GetRenderComponent()->SetSpriteColor(NewCurrRoomColor);

	FLinearColor NewNextRoomColor = UKismetMathLibrary::LinearColorLerp(NextRoomColor, CurrentRoomColor, t);
	MinimapNextRoom->GetRenderComponent()->SetSpriteColor(NewNextRoomColor);

	if (t >= 1.f) MoveEndedCurrentMinimapRoom();
}

void AMinimapActor::MoveEndedCurrentMinimapRoom()
{
	CurrentMinimapRoom = MinimapNextRoom;
	bChangeCurrentMinimapRoom = false;
	SceneCaptureLocationTimeElapsed = 0.f;
}
#pragma endregion

float AMinimapActor::easeInOutCubic(float t)
{
	return t < 0.5 ? 4 * FMath::Pow(t, 3) : 1 - FMath::Pow(-2 * t + 2, 3) / 2;
}
