// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/EnemyClasses/BaseEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"
#include "BaseEnemyAIController.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Component"));
	RootComponent = CollisionBoxComp;
	CollisionBoxComp->SetCollisionProfileName(FName(TEXT("EnemyBox")));
	CollisionBoxComp->SetLinearDamping(8.f);
	CollisionBoxComp->GetBodyInstance()->bLockXRotation = true;
	CollisionBoxComp->GetBodyInstance()->bLockYRotation = true;
	//CollisionBoxComp->SetSimulatePhysics(true);

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));

	EnemyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy Mesh Component"));
	EnemyMeshComp->SetupAttachment(CollisionBoxComp);
	EnemyMeshComp->SetCollisionProfileName(FName(TEXT("EnemyMesh")));

	BulletComp = CreateDefaultSubobject<UBulletComponent>(TEXT("Bullet Component"));
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyAIController = Cast<ABaseEnemyAIController>(GetController());
	OriginalMovementSpeed = FloatingMovement->GetMaxSpeed();
	GetWorld()->GetTimerManager().SetTimer(SpawnEnemyHandle, this, &ABaseEnemy::SetUpEnemy, 1.f, false);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanGoToLocation == false) return;

	if (TimeBounceElapsed <= TimeToReachWall)
	{
		FVector NewLoc = FMath::Lerp(OriginalLocation, StartingBounceLocation, TimeBounceElapsed / TimeToReachWall);
		SetActorLocation(NewLoc);
		TimeBounceElapsed += DeltaTime;
	}
	else
	{
		FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), BounceLocation);
		SetActorRotation(NewRot);
		bCanGoToLocation = false;
		BounceOfWalls();
	}
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::ChangeMovementSpeed(bool bChangeToOriginal)
{
	if (bChangeToOriginal == true) FloatingMovement->MaxSpeed = OriginalMovementSpeed;
	else FloatingMovement->MaxSpeed = SpeedWhenSeePlayer;
}

void ABaseEnemy::BounceOfWalls()
{
	FHitResult Hit;
	TArray<AActor*> ActorToIgnore;
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation() + GetActorForwardVector() * 100.f, GetActorLocation() + GetActorForwardVector() * 3000.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4), false, ActorToIgnore,
		EDrawDebugTrace::Persistent, Hit, true) == false) return;
	OriginalLocation = GetActorLocation();
	StartingBounceLocation = Hit.Location;
	TimeBounceElapsed = 0.f;

	BounceLocation = FMath::GetReflectionVector(GetActorForwardVector(), Hit.ImpactNormal);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Hit.Location + BounceLocation * 100.f, Hit.Location + BounceLocation * 3000.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4), false, ActorToIgnore,
		EDrawDebugTrace::Persistent, Hit, true) == false) return;
	BounceLocation = Hit.Location;

	bCanGoToLocation = true;
}

void ABaseEnemy::SetUpEnemy()
{
	if (EnemyActionWhenSpawned == ESA_BouncesOfWalls)
	{
		CollisionBoxComp->SetSimulatePhysics(false);
		BounceOfWalls();
	}
	EnemyAIController->SetUpEnemyAI(MaxRandomLocationRadius);
}

