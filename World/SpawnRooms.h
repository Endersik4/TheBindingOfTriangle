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

private:
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		class UInstancedStaticMeshComponent* FloorInstanceStaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
		class UInstancedStaticMeshComponent* WallInstanceStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Room Settings")
		int32 NumberOfFloors_X = 1;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		int32 NumberOfFloors_Y = 1;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		float DistanceBetweenFloors_X = 400.f;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		float DistanceBetweenFloors_Y = 400.f;
	UPROPERTY(EditAnywhere, Category = "Room Settings", meta = (ClampMin = "0"))
		int32 NumberOfDoors = 2;
	UPROPERTY(EditAnywhere, Category = "Room Settings")
		FRandomStream RandomDoorSeed;

	void AddWallAndDoor(bool bCanSpawnDoor, FVector RelativeLocation, FRotator RelativeRotation = FRotator(0.f));
	void SetUpDoor();
	int32 GetRandomValueForDoor(int32 WallsAmount);


	TArray<int32> NumberOfCurrentDoors;
	int32 AmountOfWalls;
	TArray<int32> Doors;


};
