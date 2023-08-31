// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TheBindingOfTriangle/Interfaces/TakeDamageInterface.h"

#include "BaseEnemy.generated.h"

UENUM(BlueprintType)
enum EEnemySpawnAction {
	ESA_WalkAimlessly,
	ESA_StandsStill,
	ESA_BouncesOfWalls,
	ESA_WalkHorizontallyVertically
};

UENUM(BlueprintType)
enum EEnemyAction {
	EA_ChasesPlayer,
	EA_ChargePlayer,
	EA_None
};

UENUM(BlueprintType)
enum EEnemyDamageType {
	EEDT_Bullets,
	EEDT_ContactDamage,
	EEDT_ContactBombDamage,
	EEDT_None
};

UENUM(BlueprintType)
enum EEnemyWhereShoot {
	EWS_Ahead,
	EWS_Player
};

UCLASS()
class THEBINDINGOFTRIANGLE_API ABaseEnemy : public APawn, public ITakeDamageInterface
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

	// Take Damage Interface
	virtual void TakeDamage(float Damage, float Impulse, FVector ImpulseDir) override;

	UFUNCTION(BlueprintImplementableEvent)
		void ChangeColorAfterHit(UMaterialInstanceDynamic* TriangleMaterial);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UBulletComponent* BulletComp;

	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	EEnemyAction GetEnemyActionSpottedPlayer() const { return EnemyActionSpottedPlayer; }
	EEnemySpawnAction GetEnemyActionWhenSpawned() const { return EnemyActionWhenSpawned; }
	bool GetShouldFocusOnPlayer() const { return bShouldFocusOnPlayer; }

	void SetStartShooting(bool bNewStartShooting) { bStartShooting = bNewStartShooting; }
	void SetCurrentRoom(class ARoom* NewRoom) { CurrentRoom = NewRoom; }

	void StopEnemyMovement(float Time);
	void ChangeMovementSpeed(bool bChangeToOriginal);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* CollisionBoxComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* EnemyMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UFloatingPawnMovement* FloatingMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		float Health = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|ActionAfterHealth")
		bool bDoActionAfterLostHealth = false;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|ActionAfterHealth", meta = (EditCondition = "bDoActionAfterLostHealth"))
		float HealthLeft = Health / 2;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|ActionAfterHealth", meta = (EditCondition = "bDoActionAfterLostHealth"))
		TArray<TEnumAsByte<EEnemyDamageType>>  ActionAfterHealth = {EEDT_Bullets};

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		float MaxRandomLocationRadius = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		TSubclassOf<class AGeometryCollectionActor> EnemyMeshFracture;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|Action When Spawned")
		TEnumAsByte<EEnemySpawnAction> EnemyActionWhenSpawned = ESA_WalkAimlessly;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|Action When Spawned", meta = (EditCondition = "EnemyActionWhenSpawned == EEnemySpawnAction::ESA_BouncesOfWalls"))
		float SpeedToReachWall = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|Action When Spawned", meta = (EditCondition = "EnemyActionWhenSpawned == EEnemySpawnAction::ESA_StandsStill"))
		bool bShouldFocusOnPlayer;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|Action When Spotted Player")
		TEnumAsByte<EEnemyAction> EnemyActionSpottedPlayer = EA_ChasesPlayer;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|Action When Spotted Player", meta = (EditCondition = "EnemyActionSpottedPlayer == EEnemyAction::EA_ChargePlayer"))
		float SpeedWhenSeePlayer = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|Damage")
		TArray<TEnumAsByte<EEnemyDamageType>> EnemyDamageTypes = { EEDT_ContactDamage };
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings|Damage", meta = (EditCondition = "EnemyDamageType == EEnemyDamageType::EEDT_Bullets"))
		TEnumAsByte<EEnemyWhereShoot> EnemyShootDirection = EWS_Player;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Settings")
		TArray<TSubclassOf<ABaseEnemy>> EnemiesToSpawnAfterDeath;

	UPROPERTY(EditAnywhere, Category = "Item Settings")
		float PopUpTime = 0.4f;


	// Room
	class ARoom* CurrentRoom;

	float OriginalMovementSpeed;

	// After enemy lost some health:
	void ActionWhenLostHealth();

	// Shoot
	bool bStartShooting;

	// Damage
	void ContactDamage(AActor* ActorToHit, const FHitResult& SweepResult);

	UMaterialInstanceDynamic* BaseEnemyDynamicMat;

	// Bouncing
	bool bCanGoToLocation;
	FVector StartingBounceLocation;
	FVector BounceLocation;
	void InterpToBounceOfWallsLocation(float DeltaTime);
	void CalculateLocationToBounceOfWalls();

	// Change mesh scale from 0.f to original scale in Time (TimeEffect)
	bool bShouldMeshPopUp = true;
	float OriginalScale;
	float PopUpTimeElapsed;
	void PopUpMesh(float Delta);

	// Spawn Enemies After Death
	void SpawnEnemiesAfterDeath();

	void SetUpEnemy();

	class ABaseEnemyAIController* EnemyAIController;
};
