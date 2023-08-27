// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class THEBINDINGOFTRIANGLE_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyAIController();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	void SetUpEnemyAI(float MaxRadius, enum EEnemyDamageType, enum EEnemyWhereShoot);
private:
	UFUNCTION()
		void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception;

	// Bullets
	bool bShouldStartShooting;

	// Charge
	bool bIsCharging;
	bool bCanChargePlayer = true;
	FTimerHandle ChargePlayerHandle;
	void SetCanChargePlayer() { bCanChargePlayer = true; }
	void ChargeFinished();
	void ChargeToPlayerLocation(FVector Location, AActor* Actor);

	// Chase The Player
	void ChasingPlayer();

	// Move the enemy only horizontally and vertically
	FTimerHandle MoveHorizontallyHandle;
	void MoveEnemyHorizontallyVertically();

	// Move enemy to random location (loop)
	float MaxRandomLocationRadius;
	FVector NextLocation;
	FTimerHandle RandomLocationHandle;
	void PickRandomLocationInRoom();

	APawn* PlayerPawn;
	class UNavigationSystemV1* NavSys;
	class ABaseEnemy* EnemyPawn;
};
