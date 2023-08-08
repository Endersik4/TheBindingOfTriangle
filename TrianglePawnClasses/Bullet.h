// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
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

private:
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* BulletMeshComp;

	FVector TrajectoryBullet;
	FBulletStruct BulletData;

	void MovementBullet(float Delta);

};
