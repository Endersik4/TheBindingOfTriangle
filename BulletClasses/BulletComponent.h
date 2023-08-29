// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletData.h"
#include "BulletComponent.generated.h"

/// <summary>
/// UBulletComponent is used for calculating bullet data and spawn bullets 
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THEBINDINGOFTRIANGLE_API UBulletComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBulletComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Settings")
		bool bShouldDrawDebugBullets = false;

	UFUNCTION(BlueprintCallable)
		void DirectionForBullets(FRotator MeshRelativeRotation);

	UFUNCTION(BlueprintCallable)
		FBulletStruct GetBulletData() const { return Bullet; }

	UFUNCTION(BlueprintCallable)
		int32 GetHoldBulletDivideCounter() const { return HoldBulletDivideCounter; }

	void Shoot(FRotator MeshRelativeRotation);
	bool HoldBullet();
	bool ShouldSkipHoldBullet();
	void SetShouldSkipHoldBullet(bool bShould) { bShouldSkipHoldBullet = bShould; }

	void SetPlayerVariables(UMaterialInstanceDynamic* DynamicMat);
	void SetEnemyOwner(class ABaseEnemy* NewOwner) { EnemyOwner = NewOwner; }
	void RestartLaser();

private:
	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		TSubclassOf<class ABullet> BulletClass;
	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		FBulletStruct Bullet;

	void SpawnBullet(FVector StartLocation, FVector DirForBullet);

	// Laser
	bool bShootLaser;
	float LaserTimeElapsed;
	FRotator PawnRotation;
	void LaserBullet(FVector StartLocation, FVector DirForLaser);
	class UNiagaraComponent* SpawnedLaserParticle;

	// Hold Bullet
	FTimerHandle HoldBulletHandle;
	float DamageBeforeHoldBullet;
	int32 HoldBulletDivideCounter = 1;
	bool bShouldSkipHoldBullet = false;
	void HoldBulletSetDamage();
	void ClearHoldBullet();

	// Enemy
	class ABaseEnemy* EnemyOwner;

	// Frequency Bullet
	bool bCanSpawnAnotherBullet = true;
	FTimerHandle SpawnAnotherBulletHandle;
	void SetCanSpawnAnotherBullet() { bCanSpawnAnotherBullet = true; }

	class ATrianglePawn* TrianglePawnPlayer;
	UMaterialInstanceDynamic* BaseTriangleDynamicMat;

};
