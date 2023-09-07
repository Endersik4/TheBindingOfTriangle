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

	MinimapSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Minimap Scene Capture"));
	MinimapSceneCapture->SetupAttachment(BoxMainComponent);
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
	SmoothSceneCaptureLocation(DeltaTime);
}

void AMinimapActor::SpawnMinimap(const TMap<FVector, FRoomStruct>& AllRooms)
{
	AllRoomsData = AllRooms;
}

void AMinimapActor::SpawnMinimapSpriteRoom(const FVector& RealRoomLocation)
{
	TArray<FVector> RoomNeighboursLocation;
	GetRoomNeighboursLocation(RoomNeighboursLocation, RealRoomLocation);

	for (int i = 0; i != 5; i++)
	{
		const FRoomStruct* CurrMinimapRoom = AllRoomsData.Find(RoomNeighboursLocation[i]);
		if (CurrMinimapRoom == nullptr) continue;

		if (i == 0)
		{
			MinimapRoomColor = GetProperRoomColor(CurrMinimapRoom->RoomType);
		}

		FVector Location = TransformRoomLocation(CurrMinimapRoom->Location);
		if (AllMinimapRooms.Find(Location)) continue;

		FColor SpriteRoomColor = GetProperRoomColor(CurrMinimapRoom->RoomType);

		APaperSpriteActor* SpawnedSpriteActor = GetWorld()->SpawnActor<APaperSpriteActor>(APaperSpriteActor::StaticClass(), Location, FRotator(0.f, 90.f, 90.f));
		if (SpawnedSpriteActor == nullptr) continue;

		if (i == 0)
		{
			CurrentMinimapRoom = SpawnedSpriteActor;
		}
		AllMinimapRooms.Add(Location, SpawnedSpriteActor);

		if (CurrMinimapRoom->RoomType == ERT_Normal) SpriteRoomColor = NotDiscoveredRoomColor;

		SpawnedSpriteActor->SetActorScale3D(FVector(0.1f));
		SpawnedSpriteActor->GetRenderComponent()->SetMobility(EComponentMobility::Movable);
		SpawnedSpriteActor->GetRenderComponent()->SetSprite(RoomPaperSprite);
		SpawnedSpriteActor->GetRenderComponent()->SetSpriteColor(SpriteRoomColor);
	}
}

void AMinimapActor::GetRoomNeighboursLocation(TArray<FVector>& RoomNeighboursLocation, FVector RealRoomLocation)
{
	RoomNeighboursLocation.Add(RealRoomLocation);
	for (int i = 0; i != 4; i++)
	{
		if (i == 0) RoomNeighboursLocation.Add(RealRoomLocation + FVector(1360.f, 0.f, 0.f));
		else if (i == 1) RoomNeighboursLocation.Add(RealRoomLocation + FVector(-1360.f, 0.f, 0.f));
		else if (i == 2) RoomNeighboursLocation.Add(RealRoomLocation + FVector(0, 2440.f, 0.f));
		else RoomNeighboursLocation.Add(RealRoomLocation + FVector(0.f, -2440.f, 0.f));
	}
}

FColor& AMinimapActor::GetProperRoomColor(const ERoomType& RoomType)
{
	if (RoomType == ERT_Award) return AwardRoomColor;
	else if (RoomType == ERT_Boss) return BossRoomColor;
	else if (RoomType == ERT_Shop) return ShopRoomColor;
	else return NormalRoomColor;
}

FVector AMinimapActor::TransformRoomLocation(const FVector& RealRoomLocation)
{
	FVector Location = RealRoomLocation;
	Location.X /= LocationDivider.X;
	Location.Y /= LocationDivider.Y;
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), Location);
}

void AMinimapActor::ChangeCurrentRoom(FVector RealRoomLocation)
{
	SpawnMinimapSpriteRoom(RealRoomLocation);
	FVector Location = TransformRoomLocation(RealRoomLocation);

	APaperSpriteActor* FoundActor = *AllMinimapRooms.Find(Location);
	if (FoundActor == nullptr || FoundActor == CurrentMinimapRoom) return;

	FoundActor->GetRenderComponent()->SetSpriteColor(CurrentRoomColor);
	MinimapNextRoom = FoundActor;
	ChangeMinimapRoom(true, Location);
}

void AMinimapActor::SetInitialRoomColor(const ERoomType& InitialRoomType)
{
	InitialRoomColor = GetProperRoomColor(InitialRoomType);
}

#pragma region ////////////////// SCENE CAPTURE /////////////////////
void AMinimapActor::ChangeMinimapRoom(bool bChangeLoc, FVector CameraLocation)
{
	bChangeSceneCaptureLocation = bChangeLoc;
	SceneCaptureStartPosition = MinimapSceneCapture->GetRelativeLocation();

	SceneCaptureEndPosition = CameraLocation;
	SceneCaptureEndPosition.Z = SceneCaptureStartPosition.Z;
}

void AMinimapActor::SmoothSceneCaptureLocation(float Delta)
{
	if (bChangeSceneCaptureLocation == false) return;

	SceneCaptureLocationTimeElapsed += Delta;
	float t = FMath::Clamp(SceneCaptureLocationTimeElapsed / SceneCaptureChangeLocationTime, 0, 1);

	t = easeInOutCubic(t);

	FVector NewLocation = FMath::Lerp(SceneCaptureStartPosition, SceneCaptureEndPosition, t);
	MinimapSceneCapture->SetRelativeLocation(NewLocation);

	FLinearColor NewCurrRoomColor = UKismetMathLibrary::LinearColorLerp(CurrentRoomColor, InitialRoomColor, t);
	CurrentMinimapRoom->GetRenderComponent()->SetSpriteColor(NewCurrRoomColor);

	FLinearColor NewNextRoomColor = UKismetMathLibrary::LinearColorLerp(MinimapRoomColor, CurrentRoomColor, t);
	MinimapNextRoom->GetRenderComponent()->SetSpriteColor(NewNextRoomColor);

	if (t >= 1.f)
	{
		CurrentMinimapRoom = MinimapNextRoom;
		bChangeSceneCaptureLocation = false;
		SceneCaptureLocationTimeElapsed = 0.f;
	}
}
#pragma endregion

float AMinimapActor::easeInOutCubic(float t)
{
	return t < 0.5 ? 4 * FMath::Pow(t, 3) : 1 - FMath::Pow(-2 * t + 2, 3) / 2;
}
