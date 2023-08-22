// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomData.h"

#include "Room.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
UCLASS()
class THEBINDINGOFTRIANGLE_API ARoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetRoomData(const FRoomStruct& Room) { RoomData = Room; }

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UStaticMeshComponent* FloorMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UStaticMeshComponent* TopWallMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UStaticMeshComponent* BottomWallMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UStaticMeshComponent* RightWallMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UStaticMeshComponent* LeftWallMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UBoxComponent* ActivateRoomBoxComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		UBoxComponent* CameraLocationBoxComp;

	UPROPERTY(VisibleAnywhere, Category = "Room Settings")
		FRoomStruct RoomData;
	UPROPERTY(EditDefaultsOnly, Category = "Room Settings")
		UStaticMesh* TopDoorWallStaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Room Settings")
		UStaticMesh* RightDoorWallStaticMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Room Settings")
		TSubclassOf<class ADoor> DoorClass;

	//TArray<class ADoor> DoorData;
	void SpawnDoors();
	FTransform GetProperTransform(int32 Index);

	class ATrianglePawn* TrianglePawn;

};
