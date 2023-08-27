// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/EnemyClasses/BaseEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"
#include "BaseEnemyAIController.h"
#include "TheBindingOfTriangle/Interfaces/TakeDamageInterface.h"

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
	EnemyAIController = Cast<ABaseEnemyAIController>(GetController());
	OriginalMovementSpeed = FloatingMovement->GetMaxSpeed();
	GetWorld()->GetTimerManager().SetTimer(SpawnEnemyHandle, this, &ABaseEnemy::SetUpEnemy, 1.f, false);
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyActionWhenSpawned == ESA_BouncesOfWalls) InterpToBounceOfWallsLocation(DeltaTime);

	if (bStartShooting == true) BulletComp->Shoot(GetActorRotation());
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
	CollisionBoxComp->AddImpulse(Impulse * ImpulseDir, NAME_None, true);

	if (Health <= 0.f) Destroy();
}

void ABaseEnemy::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (EnemyDamageType == EDT_ContactDamage) ContactDamage(OtherActor);
}

void ABaseEnemy::ContactDamage(AActor* ActorToHit)
{
	ITakeDamageInterface* DamageInterface = Cast<ITakeDamageInterface>(ActorToHit);
	if (DamageInterface == nullptr) return;

	FVector Dir = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ActorToHit->GetActorLocation()).Vector();
	DamageInterface->TakeDamage(BulletComp->GetBulletData().Damage, BulletComp->GetBulletData().Impulse, Dir);
}
#pragma endregion

void ABaseEnemy::ChangeMovementSpeed(bool bChangeToOriginal)
{
	if (bChangeToOriginal == true) FloatingMovement->MaxSpeed = OriginalMovementSpeed;
	else FloatingMovement->MaxSpeed = SpeedWhenSeePlayer;
}
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

void ABaseEnemy::SetUpEnemy()
{
	if (EnemyActionWhenSpawned == ESA_BouncesOfWalls)
	{
		EnemyActionSpottedPlayer = EA_None;
		CollisionBoxComp->SetSimulatePhysics(false);
		CalculateLocationToBounceOfWalls();
	}
	EnemyAIController->SetUpEnemyAI(MaxRandomLocationRadius, EnemyDamageType, EnemyShootDirection);
}

