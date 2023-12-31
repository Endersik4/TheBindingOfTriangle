// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/EnemyClasses/BaseEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "NavigationSystem.h"

#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"
#include "BaseEnemyAIController.h"
#include "TheBindingOfTriangle/Interfaces/TakeDamageInterface.h"
#include "TheBindingOfTriangle/World/Room.h"

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

	CollisionBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnComponentBeginOverlap);

	BaseEnemyDynamicMat = UMaterialInstanceDynamic::Create(EnemyMeshComp->GetMaterial(1), this);
	EnemyMeshComp->SetMaterial(1, BaseEnemyDynamicMat);

	EnemyAIController = Cast<ABaseEnemyAIController>(GetController());

	OriginalScale = EnemyMeshComp->GetRelativeScale3D().X;
	EnemyMeshComp->SetRelativeScale3D(FVector(0.01f));
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyActionWhenSpawned == ESA_BouncesOfWalls) InterpToBounceOfWallsLocation(DeltaTime);

	if (bStartShooting == true) BulletComp->Shoot(GetActorRotation());

	if (bShouldMeshPopUp == true) PopUpMesh(DeltaTime);
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

#pragma region //////////////// DAMAGE ////////////////////
void ABaseEnemy::TakeDamage(float Damage, float Impulse, FVector ImpulseDir)
{
	Health -= Damage;

	ActionWhenLostHealth();

	if (Health <= 0.f)
	{
		FVector Loc = EnemyMeshComp->GetComponentLocation();
		AGeometryCollectionActor* SpawnedFractureMesh = GetWorld()->SpawnActor<AGeometryCollectionActor>(EnemyMeshFracture, Loc, GetActorRotation());
		if (SpawnedFractureMesh != nullptr)
		{
			SpawnedFractureMesh->GetGeometryCollectionComponent()->ApplyExternalStrain(0, Loc, 100.f, 1.f, 1.f, 5500.f);
			SpawnedFractureMesh->GetGeometryCollectionComponent()->ApplyBreakingLinearVelocity(0, Loc);
			FVector ImpulseForce = (Impulse / 2) * (ImpulseDir - FVector(0.f, 0.f, 1.f));
			SpawnedFractureMesh->GetGeometryCollectionComponent()->AddImpulse(ImpulseForce, NAME_None, true);
		}

		SpawnEnemiesAfterDeath();

		if (CurrentRoom) CurrentRoom->AddHowManyEnemisSet(-1);

		Destroy();
		return;
	}
	if (CollisionBoxComp->IsSimulatingPhysics() == true) CollisionBoxComp->AddImpulse(Impulse * ImpulseDir, NAME_None, true);
	ChangeColorAfterHit(BaseEnemyDynamicMat);
}

void ABaseEnemy::ActionWhenLostHealth()
{
	if (bDoActionAfterLostHealth == false || Health > HealthLeft) return;

	EnemyAIController->ChangeShootingDirection(ActionAfterHealth, EnemyShootDirection);
}

void ABaseEnemy::SpawnEnemiesAfterDeath()
{
	for (TSubclassOf<ABaseEnemy> EnemyToSpawn : EnemiesToSpawnAfterDeath)
	{
		FNavLocation RandLoc;
		if (UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(GetActorLocation(), 250.f, RandLoc) == false) return;
		float Yaw = FMath::FRandRange(-180.f, 180.f);

		ABaseEnemy* SpawnedEnemy = GetWorld()->SpawnActor<ABaseEnemy>(EnemyToSpawn, RandLoc.Location, FRotator(0.f, Yaw, 0.f));
		if (SpawnedEnemy)
		{
			SpawnedEnemy->SetCurrentRoom(CurrentRoom);
			if (CurrentRoom == nullptr) continue;
			CurrentRoom->AddHowManyEnemisSet(1);
		}
	}
}

void ABaseEnemy::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (EnemyDamageTypes.FindByKey(EEDT_ContactDamage) || EnemyDamageTypes.FindByKey(EEDT_ContactBombDamage)) ContactDamage(OtherActor, SweepResult);
}

void ABaseEnemy::ContactDamage(AActor* ActorToHit, const FHitResult& SweepResult)
{
	if (EnemyDamageTypes.FindByKey(EEDT_ContactBombDamage))
	{
		Health = 0.01f;
		BulletComp->SpawnImmediatelyBombBullet();	
		return;
	}

	ITakeDamageInterface* DamageInterface = Cast<ITakeDamageInterface>(ActorToHit);
	if (DamageInterface == nullptr) return;

	FVector Dir = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ActorToHit->GetActorLocation()).Vector();
	DamageInterface->TakeDamage(BulletComp->GetBulletData().Damage, BulletComp->GetBulletData().Impulse, Dir);
}
#pragma endregion

#pragma region /////////////// BOUNCE OF WALLS //////////////////

void ABaseEnemy::CalculateLocationToBounceOfWalls()
{
	FHitResult Hit;
	TArray<AActor*> ActorToIgnore;

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation() + GetActorForwardVector() * 100.f, GetActorLocation() + GetActorForwardVector() * 3000.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4), false, ActorToIgnore,
		EDrawDebugTrace::None, Hit, true) == false) return;

	StartingBounceLocation = Hit.Location - GetActorForwardVector() * 100.f; // The enemy will stop moving before hitting the wall

	BounceLocation = FMath::GetReflectionVector(GetActorForwardVector(), Hit.ImpactNormal);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), Hit.Location + BounceLocation * 100.f, Hit.Location + BounceLocation * 3000.f, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4), false, ActorToIgnore,
		EDrawDebugTrace::None, Hit, true) == false) return;

	BounceLocation = Hit.Location;
	bCanGoToLocation = true;
}


void ABaseEnemy::InterpToBounceOfWallsLocation(float DeltaTime)
{
	if (bCanGoToLocation == false) return;

	FVector NewLoc = FMath::VInterpConstantTo(GetActorLocation(), StartingBounceLocation, DeltaTime, SpeedToReachWall * 10);
	SetActorLocation(NewLoc);

	if ((GetActorLocation().Equals(StartingBounceLocation, 10.f)) == false) return;

	FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), BounceLocation);
	SetActorRotation(NewRot);
	bCanGoToLocation = false;
	CalculateLocationToBounceOfWalls();
}

#pragma endregion

void ABaseEnemy::PopUpMesh(float Delta)
{
	if (PopUpTimeElapsed <= PopUpTime)
	{
		float NewScale = FMath::Lerp(0.01f, OriginalScale, PopUpTimeElapsed / PopUpTime);
		EnemyMeshComp->SetRelativeScale3D(FVector(NewScale));
		PopUpTimeElapsed += Delta;
	}
	else
	{
		EnemyMeshComp->SetRelativeScale3D(FVector(OriginalScale));

		// Activate enemy after pop up effect
		SetUpEnemy();

		bShouldMeshPopUp = false;
	}
}

void ABaseEnemy::SetUpEnemy()
{
	BulletComp->SetEnemyOwner(this);
	OriginalMovementSpeed = FloatingMovement->GetMaxSpeed();

	if (EnemyActionWhenSpawned == ESA_BouncesOfWalls)
	{
		EnemyActionSpottedPlayer = EA_None;
		CollisionBoxComp->SetSimulatePhysics(false);
		CalculateLocationToBounceOfWalls();
	}
	EnemyAIController->SetUpEnemyAI(MaxRandomLocationRadius, EnemyDamageTypes, EnemyShootDirection);
}

void ABaseEnemy::ChangeMovementSpeed(bool bChangeToOriginal)
{
	if (bChangeToOriginal == true) FloatingMovement->MaxSpeed = OriginalMovementSpeed;
	else FloatingMovement->MaxSpeed = SpeedWhenSeePlayer;
}

void ABaseEnemy::StopEnemyMovement(float Time)
{
	EnemyAIController->StopMovementForTime(Time);
}
