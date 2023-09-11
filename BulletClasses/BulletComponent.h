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
		void CalculateAndSpawnBullets(FRotator MeshRelativeRotation);

	UFUNCTION(BlueprintCallable)
		FBulletStruct& GetBulletData() { return Bullet; }

	UFUNCTION(BlueprintCallable)
		int32 GetHoldBulletDivideCounter() const { return HoldBulletDivideCounter; }

	// Bullets
	void Shoot(FRotator MeshRelativeRotation);
	bool WasHoldBulletActivated();
	void SpawnImmediatelyBombBullet();

	// Hold bullet
	bool ShouldSkipHoldBullet();
	void SetShouldSkipHoldBullet(bool bShould) { bShouldSkipHoldBullet = bShould; }

	// Laser
	void TurnOffLaser();

	// setters
	void SetPlayerVariables(UMaterialInstanceDynamic* DynamicMat);
	void SetEnemyOwner(class ABaseEnemy* NewOwner) { EnemyOwner = NewOwner; }
private:
	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		FBulletStruct Bullet;

	void SpawnBullet(FVector StartLocation, FVector DirForBullet);

	FRotator PawnRotation;

	// Laser
	bool bIsLaserActive;
	float LaserTimeElapsed;
	void LaserDamageRaycast(FVector StartLocation, FVector DirForLaser);
	void ShootBulletAsLaser(const FVector & BulletSpawnLocation, const FVector & BulletShootDirection);
	void TurnOffLaserAfterTime(float Delta);
	class UNiagaraComponent* SpawnedLaserParticle;

	// Hold Bullet
	FTimerHandle HoldBulletHandle;
	float DamageBeforeHoldBullet;
	int32 HoldBulletDivideCounter = 1;
	bool bShouldSkipHoldBullet = false;
	void HoldBulletSetDamage();
	void TurnOffHoldBullet();

	// Enemy
	class ABaseEnemy* EnemyOwner;

	// Frequency Bullet
	bool bCanShoot = true;
	FTimerHandle SpawnAnotherBulletHandle;
	void SetCanSpawnAnotherBullet() { bCanShoot = true; }

	class ATrianglePawn* TrianglePawnPlayer;
	UMaterialInstanceDynamic* BaseTriangleDynamicMat;

};
