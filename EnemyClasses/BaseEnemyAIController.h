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


protected:
	virtual void BeginPlay() override;
public:
	ABaseEnemyAIController();

private:

	UFUNCTION()
		void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBehaviorTree* AIBehaviour;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception;
	
};
