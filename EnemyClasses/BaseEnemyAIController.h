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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		FVector RoomLocation;

	void SetRoomLocation(FVector NewLoc);
private:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION()
		void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception;

	FVector NextLocation;
	void PickRandomLocationInRoom();
	FTimerHandle MoveHandle;

	//class UNavigationSystemV1* NavSys;
};
