// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnRooms.generated.h"

UCLASS()
class THEBINDINGOFTRIANGLE_API ASpawnRooms : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnRooms();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void SetUpRoom();

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		class UInstancedStaticMeshComponent* FloorInstanceStaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		class UInstancedStaticMeshComponent* WallInstanceStaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		class UBoxComponent* ActivateRoomBoxComp;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		class UBoxComponent* CameraLocationBox;

	UPROPERTY(EditAnywhere, Category = "Room Settings")
		int32 NumberOfFloors_X = 1;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		int32 NumberOfFloors_Y = 1;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		float DistanceBetweenFloors_X = 400.f;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		float DistanceBetweenFloors_Y = 400.f;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		float CameraChangeLocationTime = 0.3f;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		FRandomStream RandomDoorSeed;

	void AddWallAndDoor(bool bAddDoor, FVector RelativeLocation, FRotator RelativeRotation = FRotator(0.f));

	class ATrianglePawn* TrianglePawn;

	bool bChangeCameraLocation;
	float CameraLocationTimeElapsed;
	FVector CameraStartPosition;
	void SmoothCameraLocation(float Delta);

	float easeInOutCubic(float t);
};
