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

	void SpawnMinimap(const TMap<FVector, FRoomStruct> & AllRooms);
	void SpawnMinimapSpriteRoom(const FVector  &RealRoomLocation);
	void ChangeCurrentRoom(FVector RealRoomLocation);
	void SetInitialRoomColor(const ERoomType& InitialRoomType);

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
		FColor NotDiscoveredRoomColor = FColor::Silver;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings|Minimap Room Colors")
		FColor NormalRoomColor = FColor::Blue;
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

	FColor& GetProperRoomColor(const ERoomType &RoomType);
	FVector TransformRoomLocation(const FVector & RealRoomLocation);

	//
	class APaperSpriteActor* MinimapNextRoom;
	void GetRoomNeighboursLocation(TArray<FVector>& RoomNeighboursLocation, FVector RealRoomLocation);

	FColor InitialRoomColor;

	// Move Scene Capture
	FColor MinimapRoomColor;
	bool bChangeSceneCaptureLocation;
	float SceneCaptureLocationTimeElapsed;
	FVector SceneCaptureStartPosition;
	FVector SceneCaptureEndPosition;
	void ChangeMinimapRoom(bool bChangeLoc, FVector CameraLocation);
	void SmoothSceneCaptureLocation(float Delta);

	// Math
	float easeInOutCubic(float t);
};
