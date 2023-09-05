// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/MinimapActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PaperSpriteActor.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMinimapActor::AMinimapActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Main Scene Component"));
	RootComponent = MainSceneComponent;

	MinimapSceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Minimap Scene Capture"));
	MinimapSceneCapture->SetupAttachment(MainSceneComponent);
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

}

void AMinimapActor::SpawnMinimap(const TMap<FVector, FRoomStruct>& AllRooms)
{
	if (RoomPaperSprite == nullptr) return;
	AllRoomsData = AllRooms;

	TArray<FRoomStruct> RoomsData;
	AllRoomsData.GenerateValueArray(RoomsData);

	for (const FRoomStruct& Room : RoomsData)
	{
		FVector Location = TransformRoomLocation(Room.Location);

		const FColor & SpriteRoomColor = GetProperRoomColor(Room.RoomType);

		APaperSpriteActor* SpawnedSpriteActor = GetWorld()->SpawnActor<APaperSpriteActor>(APaperSpriteActor::StaticClass(), Location, FRotator(0.f, 90.f, 90.f));
		if (SpawnedSpriteActor == nullptr) continue;

		if (Room.RoomType == ERT_Spawn) CurrentMinimapRoom = SpawnedSpriteActor;
		AllMinimapRooms.Add(Location, SpawnedSpriteActor);

		SpawnedSpriteActor->SetActorScale3D(FVector(0.1f));
		SpawnedSpriteActor->GetRenderComponent()->SetMobility(EComponentMobility::Movable);
		SpawnedSpriteActor->GetRenderComponent()->SetSprite(RoomPaperSprite);
		SpawnedSpriteActor->GetRenderComponent()->SetSpriteColor(SpriteRoomColor);
	}
}

FColor& AMinimapActor::GetProperRoomColor(const ERoomType& RoomType)
{
	if (RoomType == ERT_Award) return AwardRoomColor;
	else if (RoomType == ERT_Boss) return BossRoomColor;
	else if (RoomType == ERT_Shop) return ShopRoomColor;
	else if (RoomType == ERT_Spawn) return SpawnRoomColor;
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
	FVector Location = TransformRoomLocation(RealRoomLocation);

	APaperSpriteActor* FoundActor = *AllMinimapRooms.Find(Location);
	if (FoundActor == nullptr || FoundActor == CurrentMinimapRoom) return;

	CurrentMinimapRoom->GetRenderComponent()->SetSpriteColor(NormalRoomColor);
	FoundActor->GetRenderComponent()->SetSpriteColor(SpawnRoomColor);
	CurrentMinimapRoom = FoundActor;

	MinimapSceneCapture->SetRelativeLocation(FVector(Location.X, Location.Y, MinimapSceneCapture->GetRelativeLocation().Z));
}

