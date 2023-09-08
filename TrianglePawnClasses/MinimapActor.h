// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheBindingOfTriangle/World/RoomData.h"
#include "MinimapActor.generated.h"

class UPaperSprite;

UCLASS()
class THEBINDINGOFTRIANGLE_API AMinimapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinimapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetAllRoomsData(const TMap<FVector, FRoomStruct>& AllRooms) { AllRoomsData = AllRooms; }
	void SetDistanceBetweenRooms(FVector2D NewDistance) { DistanceBetweenRooms = NewDistance; }
	void SetInitialRoomColor(const ERoomType& InitialRoomType);

	void MakeRoomMinimapWithNeighbours(const FVector  &RealRoomLocation);
	void PlayerHasMovedToNextRoom(FVector RealRoomLocation);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class USceneCaptureComponent2D* MinimapSceneCapture;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* BoxMainComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		FVector2D LocationDivider = FVector2D(23.f, 28.f);
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		UPaperSprite* RoomPaperSprite;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings|Minimap Room Colors")
		FColor CurrentRoomColor = FColor::Green;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings|Minimap Room Colors")
		FColor NotDiscoveredRoomColor = FColor::Blue;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings|Minimap Room Colors")
		FColor NormalRoomColor = FColor::Cyan;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings|Minimap Room Colors")
		FColor BossRoomColor = FColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings|Minimap Room Colors")
		FColor AwardRoomColor = FColor::Yellow;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings|Minimap Room Colors")
		FColor ShopRoomColor = FColor::Magenta;

	UPROPERTY(EditAnywhere, Category = "Minimap Settings")
		float SceneCaptureChangeLocationTime = 0.4f;

	TMap<FVector, FRoomStruct> AllRoomsData;
	TMap<FVector, class APaperSpriteActor*> AllMinimapRooms;
	class APaperSpriteActor* CurrentMinimapRoom;

	// Transform Real Room Location to local of this actor
	FVector TransformRoomLocation(const FVector & RealRoomLocation);

	//
	void SpawnRoomSprite(const FVector& SpawnLocation, const FRoomStruct* RoomData, bool bFirstMinimapRoom);
	FColor& GetProperRoomColor(const ERoomType& RoomType, bool bFirstTimeSpawned = false);

	class APaperSpriteActor* MinimapNextRoom;
	FVector2D DistanceBetweenRooms;
	void GetRoomNeighboursLocation(TArray<FVector>& RoomNeighboursLocation, FVector RealRoomLocation);

	FColor OriginalRoomColor;

	// Move Scene Capture
	bool bChangeCurrentMinimapRoom;
	float SceneCaptureLocationTimeElapsed;
	FColor NextRoomColor;
	FVector SceneCaptureStartPosition;
	FVector SceneCaptureEndPosition;

	void MoveSceneCapture(bool bChangeLoc, FVector CameraLocation);
	void MoveEndedCurrentMinimapRoom();
	void MoveCurrentMinimapRoom(float Delta);

	// Math
	float easeInOutCubic(float t);
};
