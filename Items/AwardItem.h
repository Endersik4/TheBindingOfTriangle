// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheBindingOfTriangle/Items/PickableItem.h"
#include "TheBindingOfTriangle/BulletClasses/BulletData.h"
#include "AwardItem.generated.h"

UENUM(BlueprintType)
enum EAwardAction {
	EAA_Multiply,
	EAA_Divide,
	EAA_Replace,
	EAA_ReplaceBulletClass,
	EAA_ReplaceBulletType,
	EAA_ReplaceWayCurve,
	EAA_ReplaceShouldBack,
	EAA_ReplaceHoldBullet
};

UCLASS()
class THEBINDINGOFTRIANGLE_API AAwardItem : public APickableItem
{
	GENERATED_BODY()

public:
	AAwardItem();

	virtual void TakeItem(class ATrianglePawn* TrianglePawn) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Award Settings")
		TArray<TEnumAsByte<EAwardAction>> AwardAction;
	UPROPERTY(EditDefaultsOnly, Category = "Award Settings")
		FText AwardDescription;
	UPROPERTY(EditDefaultsOnly, Category = "Award Settings")
		FBulletStruct BulletData;
	UPROPERTY(EditDefaultsOnly, Category = "Award Settings")
		UTexture2D* AwardImageIcon;

};
