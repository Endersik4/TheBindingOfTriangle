// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "TheBindingOfTriangle/BulletClasses/BulletData.h"
#include "Bullet.generated.h"

UCLASS()
class THEBINDINGOFTRIANGLE_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTrajectoryBullet(FVector NewTrajectory) { TrajectoryBullet = NewTrajectory; }
	void SetBulletData(FBulletStruct NewData) { BulletData = NewData; }
	void SetBulletScale(int32 NewCounter);

	// Offset Timeline
	UFUNCTION()
		void OffsetTimelineProgress(float Value);
	void SetOffsetTimeline();

	void CallExplodeBulletBomb() { HitAsBombBullet(); }

private:
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* BulletMeshComp;

	FBulletStruct BulletData;
	FVector BulletScale;

	// Bomb 
	void HitAsBombBullet();

	// Offset Timeline
	float CurveOffset;
	FTimeline CurveTimeline;
	void ShouldUseCurveOffset(FVector& LocAfterOffset);

	// Movement
	float TrackBulletDistance;
	float BulletFallingDown = 300.f;
	FVector TrajectoryBullet;
	void MovementBullet(float Delta);
	void SetScaleWhileDroppingDown(float Delta);

	// Hold Bullet

	// Bullet Go Back
	bool ShouldBulletGoBack();

	
};
