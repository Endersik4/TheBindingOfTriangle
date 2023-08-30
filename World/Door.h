// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomData.h"
#include "Door.generated.h"


UCLASS()
class THEBINDINGOFTRIANGLE_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable)
		void ChangeDoor();

	void SetDoorType(EDoorType NewType) { DoorType = NewType; }
	void SetDoorStatus(EDoorStatus NewStatus);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* DoorMeshComp;

	UPROPERTY(EditAnywhere, Category = "Door settings")
		TEnumAsByte<EDoorType> DoorType = EDoorType::EDT_Room;
	UPROPERTY(EditAnywhere, Category = "Door settings")
		TEnumAsByte<EDoorStatus> DoorStatus = EDoorStatus::EDS_Open;
	UPROPERTY(EditAnywhere, Category = "Door settings", meta = (EditCondition = "DoorStatus == EDoorStatus::EDS_KeyRequired", EditConditionHides))
		float MaxDistanceForOpenDoor = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Door settings|Door Type")
		UMaterialInstance* DoorRoomMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Door settings|Door Type")
		UMaterialInstance* DoorBossMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Door settings|Door Type")
		UMaterialInstance* DoorAwardMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Door settings|Door Type")
		UMaterialInstance* DoorShopMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Door settings|Door Status")
		UStaticMesh* DoorOpenMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Door settings|Door Status")
		UStaticMesh* DoorClosedMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Door settings|Door Status")
		UStaticMesh* DoorKeyMesh;

	bool CheckIfPlayerIsNear();

	EDoorStatus PreviousDoorStatus;

	bool bIsDoorActive = true;
	void OpenDoorWithKey();

	class ATrianglePawn* TrianglePawn;
};
