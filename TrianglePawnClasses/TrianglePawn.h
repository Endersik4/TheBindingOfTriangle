// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TrianglePawn.generated.h"

USTRUCT(BlueprintType)
struct FBulletStruct {

	GENERATED_USTRUCT_BODY();

	// After Distance bullet will be destroyed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Distance = 300.f;
	// How fast bullet will go 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Speed = 300.f;
	// How many bullets will be spawned that is facing the same directions after given time (FrequencyTime)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float FrequencyTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Damage = 300.f;
	// Should use Way Curve when spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bUseWayCurve = false;
	// Curve for relative additional movement to the bullet, for example: instead of going straight line bullet can waving 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (EditCondition = "bUseWayCurve", EditConditionHides))
		UCurveFloat* WayCurve = nullptr;
	// Should Bullet back once the distance is reached and after this reach the distance again 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bShouldBack = false;
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

	FBulletStruct()
	{
		Distance = 0.f;
		Speed = 50.f;
		FrequencyTime = 0.2f;
		Damage = 0.f;
		bUseWayCurve = false;
		WayCurve = nullptr;
		bShouldBack = false;
		Amount = 1;
		DegreeBetween = 45.f;
		CirceRadius = 100.f;
		CirceAngle = 0.f;
	}
};

UCLASS()
class THEBINDINGOFTRIANGLE_API ATrianglePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATrianglePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void SpawnDebugBullet();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* TriangleCapsuleComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* TriangleMeshComp;
	UPROPERTY(EditAnywhere, Category = "Components")
		class ACameraActor* TriangleCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float MovementForce = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float CounterMovementForce = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		TSubclassOf<class ABullet> BulletClass;
	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		bool bShouldDrawDebugBullets = false;
	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		FBulletStruct Bullet;

	// Movement
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	FVector CounterMovement();

	void Shoot_Right(float Axis);
	void Shoot_Forward(float Axis);

	// Frequency Bullet
	bool bCanSpawnAnotherBullet = true;
	FTimerHandle SpawnAnotherBulletHandle;
	void SetCanSpawnAnotherBullet() { bCanSpawnAnotherBullet = true; }

	FRotator* CurrentRotation;

};
