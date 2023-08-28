// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/EnemyClasses/BaseEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "NavigationSystem.h"

#include "BaseEnemy.h"

ABaseEnemyAIController::ABaseEnemyAIController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIEnemyPerception"));

	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseEnemyAIController::HandleTargetPerceptionUpdated);
}

void ABaseEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (EnemyPawn == nullptr || PlayerPawn == nullptr) return;

	ChargeToPlayerLocation(Stimulus.StimulusLocation, Actor);

	ChasingPlayer();

	if (EnemyPawn->GetEnemyActionWhenSpawned() == ESA_StandsStill) SetFocus(Actor);

	if (bShouldStartShooting == true) EnemyPawn->SetStartShooting(true);
}

void ABaseEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (EnemyPawn == nullptr) return;

	ChargeFinished();
}

void ABaseEnemyAIController::ChasingPlayer()
{
	if (EnemyPawn->GetEnemyActionSpottedPlayer() != EA_ChasesPlayer || PlayerPawn == nullptr) return;

	GetWorld()->GetTimerManager().ClearTimer(RandomLocationHandle);
	MoveToActor(PlayerPawn, -1.f, false);
}

#pragma region /////////////// CHARGE TO PLAYER /////////////////////
void ABaseEnemyAIController::ChargeToPlayerLocation(FVector Location, AActor* Actor)
{
	if (EnemyPawn->GetEnemyActionSpottedPlayer() != EA_ChargePlayer || bCanChargePlayer != true || Actor != PlayerPawn) return;

	StopMovement();
	GetWorld()->GetTimerManager().ClearTimer(RandomLocationHandle);

	EnemyPawn->ChangeMovementSpeed(false);
	MoveToLocation(Location, 10.f, false);
	bCanChargePlayer = false;
	bIsCharging = true;
}


void ABaseEnemyAIController::ChargeFinished()
{
	if (EnemyPawn->GetEnemyActionSpottedPlayer() != EA_ChargePlayer || bIsCharging != true) return;

	EnemyPawn->ChangeMovementSpeed(true);
	bIsCharging = false;
	GetWorld()->GetTimerManager().SetTimer(ChargePlayerHandle, this, &ABaseEnemyAIController::SetCanChargePlayer, 0.3f, false);

	GetWorld()->GetTimerManager().SetTimer(RandomLocationHandle, this, &ABaseEnemyAIController::PickRandomLocationInRoom, FMath::FRandRange(1.f, 3.f), true, 0.f);
}
#pragma endregion

void ABaseEnemyAIController::PickRandomLocationInRoom()
{
	FNavLocation RandLoc;
	if (NavSys->GetRandomReachablePointInRadius(EnemyPawn->GetActorLocation(), MaxRandomLocationRadius, RandLoc) == false) return;

	FVector RandLocation = RandLoc.Location;
	RandLocation.Z = GetPawn()->GetActorLocation().Z;
	MoveToLocation(RandLocation, 100.f, false);
}

void ABaseEnemyAIController::MoveEnemyHorizontallyVertically()
{
	int32 DirChoice = FMath::RandRange(1,4);
	FVector Dir;
	switch (DirChoice)
	{
	case 1: Dir = FVector(1.f,0.f,0.f); break;
	case 2: Dir = FVector(-1.f, 0.f, 0.f); break;
	case 3: Dir = FVector(0.f, 1.f, 0.f); break;
	case 4: Dir = FVector(0.f, -1.f, 0.f); break;
	}

	float Distance = FMath::FRandRange(300.f, 1200.f);
	FVector RandLocation = EnemyPawn->GetActorLocation() + Distance * Dir;
	MoveToLocation(RandLocation, 10.f, false, true);
}

void ABaseEnemyAIController::SetUpEnemyAI(float MaxRadius, EEnemyDamageType EnemyDamageType, EEnemyWhereShoot EnemyWhereToShoot)
{
	MaxRandomLocationRadius = MaxRadius;
	NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	EnemyPawn = Cast<ABaseEnemy>(GetPawn());
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (EnemyPawn == nullptr) return;

	if (EnemyPawn->GetEnemyActionWhenSpawned() == ESA_WalkAimlessly)
	{
		GetWorld()->GetTimerManager().SetTimer(RandomLocationHandle, this, &ABaseEnemyAIController::PickRandomLocationInRoom, FMath::FRandRange(1.f, 3.f), true, 0.f);
	}
	else if (EnemyPawn->GetEnemyActionWhenSpawned() == ESA_StandsStill)
	{
		if (EnemyPawn->GetShouldFocusOnPlayer()) SetFocus(PlayerPawn);
	}
	else if (EnemyPawn->GetEnemyActionWhenSpawned() == ESA_WalkHorizontallyVertically)
	{
		GetWorld()->GetTimerManager().SetTimer(MoveHorizontallyHandle, this, &ABaseEnemyAIController::MoveEnemyHorizontallyVertically, FMath::FRandRange(2.f, 6.f), true, 0.1f);
	}

	if (EnemyDamageType == EDT_Bullets && EnemyWhereToShoot == EWS_Player) bShouldStartShooting = true;
	else if (EnemyDamageType == EDT_Bullets && EnemyWhereToShoot == EWS_Ahead) EnemyPawn->SetStartShooting(true);
}
