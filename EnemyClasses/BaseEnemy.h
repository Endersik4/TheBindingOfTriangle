// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum EEnemyAction {
	EA_WalkAimlessly,
	EA_StandsStill,
	EA_ChasesPlayer,
	EA_ChargePlayer
};

UENUM(BlueprintType)
enum EEnemyWalkingType {
	EWT_Normal,
	EWT_Jumping,
	EWT_BouncesOfWalls
};

UENUM(BlueprintType)
enum EEnemyDamageType {
	EDT_Bullets,
	EDT_ContactDamage,

};

UENUM(BlueprintType)
enum EEnemyWhereShoot {
	EWS_Ahead,
	EWS_Player
};

UCLASS()
class THEBINDINGOFTRIANGLE_API ABaseEnemy : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UBulletComponent* BulletComp;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* CollisionBoxComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* EnemyMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UFloatingPawnMovement* FloatingMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		float RangeToSpotThePlayer = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		FVector RoomLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		TEnumAsByte<EEnemyAction> EnemyActionWhenSpawned = EA_WalkAimlessly;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		TEnumAsByte<EEnemyAction> EnemyActionSpottedPlayer = EA_ChasesPlayer;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings", meta = (EditCondition = "EnemyActionWhenSpawned == EEnemyAction::EA_WalkAimlessly"))
		TEnumAsByte<EEnemyWalkingType> EnemyWalkingType = EWT_Normal;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		TEnumAsByte<EEnemyDamageType> EnemyDamageType = EDT_ContactDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings", meta = (EditCondition = "EnemyDamageType == EEnemyDamageType::EDT_Bullets"))
		TEnumAsByte<EEnemyWhereShoot> EnemyShootDirection = EWS_Player;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		TArray<TSubclassOf<ABaseEnemy>> EnemiesToSpawnAfterDeath;

	FTimerHandle SpawnEnemyHandle;
	void SetUpEnemy();

	class ABaseEnemyAIController* EnemyAIController;
};
