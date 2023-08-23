// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletData.generated.h"

USTRUCT(BlueprintType)
struct FBulletStruct {

	GENERATED_USTRUCT_BODY();

	// After Distance bullet will be destroyed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Damage = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Distance = 900.f;
	// How fast bullet will go 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Speed = 1400.f;
	// How many bullets will be spawned that is facing the same directions after given time (FrequencyTime)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float FrequencyTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Impulse = 200.f;
	// Should use Way Curve when spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bUseWayCurve = false;
	// Curve for relative additional movement to the bullet, for example: instead of going straight line bullet can waving 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (EditCondition = "bUseWayCurve", EditConditionHides))
		UCurveFloat* WayCurve = nullptr;
	// Should Bullet back once the distance is reached and after this reach the distance again 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bShouldBack = false;
	// Longer button presses cause more bullet damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bHoldBullet = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (EditCondition = "bHoldBullet", EditConditionHides))
		float HoldBulletTime = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (EditCondition = "bHoldBullet", EditConditionHides))
		FLinearColor HoldBulletTriangleColor = FLinearColor::Yellow;
	// How many bullets can be spawned at the same time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once")
		int32 Amount = 1;
	// Angle between bullets when there is more then 1 bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0"))
		float DegreeBetween = 45.f;
	// Radius of Circe that bullets will be spawned on the edges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once")
		float CirceRadius = 100.f;
	// How much rotate circe around player 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once")
		float CirceAngle = 0.f;
};

/**
 * 
 */
class THEBINDINGOFTRIANGLE_API BulletData
{

};
