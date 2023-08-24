// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/EnemyClasses/BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "NavigationSystem.h"

ABaseEnemyAIController::ABaseEnemyAIController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIEnemyPerception"));

	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseEnemyAIController::HandleTargetPerceptionUpdated);
}

void ABaseEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ABaseEnemyAIController::OnMoveCompleted);
}

void ABaseEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("COMPETED %s"), *Result.ToString());
	
}

void ABaseEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
}

void ABaseEnemyAIController::PickRandomLocationInRoom()
{
	
	FNavLocation RandLoc;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys->GetRandomReachablePointInRadius(RoomLocation, 1500.f, RandLoc) == false) return;
	FVector RandLocation = RandLoc.Location;
	RandLocation.Z = GetPawn()->GetActorLocation().Z;
	MoveToLocation(RandLocation, 100.f, false);
}

void ABaseEnemyAIController::SetRoomLocation(FVector NewLoc)
{
	RoomLocation = NewLoc;
	//NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	PickRandomLocationInRoom();
	GetWorld()->GetTimerManager().SetTimer(MoveHandle, this, &ABaseEnemyAIController::PickRandomLocationInRoom, 1.f, true);
}

