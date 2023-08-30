// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChestWithItem.generated.h"

UENUM(BlueprintType)
enum EChestType {
	ECT_KeyRequired,
	ECT_WithoutKey
};

UCLASS()
class THEBINDINGOFTRIANGLE_API AChestWithItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChestWithItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* ChestSkeletalMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* ChestBoxComp;

	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
		TEnumAsByte<EChestType> ChestType = ECT_KeyRequired;
	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
		TArray<TSubclassOf<class APickableItem>> ListOfItems;
	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
		UTexture2D* OpenLockTexture;
	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
		UAnimationAsset* ChestOpenAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
		float TimeEffect = 0.2f;

	// Change mesh scale from 0.f to original scale in Time (TimeEffect)
	bool bShouldChangeMeshScale = true;
	float OriginalScale;
	float TimeElapsed;
	void ChangeMeshScale(float Delta);

	bool bIsOpen;
	UMaterialInstanceDynamic* LockChestMaterial;
	bool CanOpenChest(AActor* OtherActor);
	void SpawnRandomItem();

};
