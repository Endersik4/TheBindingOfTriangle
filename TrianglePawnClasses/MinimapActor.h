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
	void ChangeCurrentRoom(FVector RealRoomLocation);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USceneComponent* MainSceneComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USceneCaptureComponent2D* MinimapSceneCapture;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		FVector2D LocationDivider = FVector2D(10.f, 16.f);
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		UPaperSprite* RoomPaperSprite;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		FColor NormalRoomColor = FColor::Blue;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		FColor BossRoomColor = FColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		FColor AwardRoomColor = FColor::Yellow;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		FColor ShopRoomColor = FColor::Magenta;
	UPROPERTY(EditDefaultsOnly, Category = "Minimap Settings")
		FColor SpawnRoomColor = FColor::Green;

	TMap<FVector, FRoomStruct> AllRoomsData;
	TMap<FVector, class APaperSpriteActor*> AllMinimapRooms;
	class APaperSpriteActor* CurrentMinimapRoom;

	FColor& GetProperRoomColor(const ERoomType &RoomType);
	FVector TransformRoomLocation(const FVector & RealRoomLocation);

	// Move camera
	bool bChangeCameraLocation;
	float CameraLocationTimeElapsed;
	FVector CameraStartPosition;
	FVector CameraEndPosition;
	void SmoothCameraLocation(float Delta);
};
