// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/Bullet.h"
#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "TheBindingOfTriangle/EnemyClasses/BaseEnemy.h"

// Sets default values for this component's properties
UBulletComponent::UBulletComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UBulletComponent::BeginPlay()
{
	Super::BeginPlay();

	DamageBeforeHoldBullet = Bullet.Damage;
}

// Called every frame
void UBulletComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TurnOffLaserAfterTime(DeltaTime);
}

#pragma region /////////////// SHOOT /////////////////////
void UBulletComponent::Shoot(FRotator MeshRelativeRotation)
{
	if (bCanShoot == false) return;

	bool bShouldTurnOffHoldBullet = Bullet.bShootHoldBulletAfterTime == true && HoldBulletDivideCounter < 8 && GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle) == true;
	if (bShouldTurnOffHoldBullet)
	{
		TurnOffHoldBullet();
		return;
	}
	
	CalculateAndSpawnBullets(MeshRelativeRotation);
	TurnOffHoldBullet();

	// Wait FrequencyTime to fire another bullet
	bCanShoot = false;
	GetWorld()->GetTimerManager().SetTimer(SpawnAnotherBulletHandle, this, &UBulletComponent::SetCanSpawnAnotherBullet, Bullet.FrequencyTime, false);
}
#pragma endregion

#pragma region ////////////// SPAWN BULLET ////////////
void UBulletComponent::CalculateAndSpawnBullets(FRotator MeshRelativeRotation)
{
	if (bShouldDrawDebugBullets == true) UKismetSystemLibrary::DrawDebugCircle(GetWorld(), GetOwner()->GetActorLocation(), Bullet.CirceRadius, 34, FLinearColor::White, 999.f, 1.f, FVector(0.f, 1.f, 0.f), FVector(1.0f, 0.f, 0.f));
	PawnRotation = MeshRelativeRotation;

	for (int i = 0; i != Bullet.Amount; i++)
	{
		// Calculate Start Location and bullet Direction
		float angReg = FMath::DegreesToRadians(Bullet.DegreeBetween * i + Bullet.CirceAngle + PawnRotation.Yaw);
		FVector BulletLocation = FVector(FMath::Cos(angReg) * Bullet.CirceRadius, FMath::Sin(angReg) * Bullet.CirceRadius, 0) + GetOwner()->GetActorLocation();
		FVector BulletTrajectory = FVector(FMath::Cos(angReg), FMath::Sin(angReg), 0);

		if (bShouldDrawDebugBullets == true)
		{
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), BulletLocation, 5.f, 6, FLinearColor::Red, 999.f, 2.f);
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), BulletLocation, BulletLocation + BulletTrajectory * 100.f, FLinearColor::Red, 999.f, 1.f);
		}

		if (Bullet.TypeOfBullet == ETB_Laser)
		{
			ShootBulletAsLaser(BulletLocation, BulletTrajectory);
		}	
		else SpawnBullet(BulletLocation, BulletTrajectory);
	}
}

void UBulletComponent::SpawnBullet(FVector StartLocation, FVector DirForBullet)
{
	ABullet* BulletActor = GetWorld()->SpawnActor<ABullet>(Bullet.BulletClass, StartLocation, PawnRotation - FRotator(0.f,90.f,0.f));
	if (BulletActor == nullptr) return;

	BulletActor->SetTrajectoryBullet(DirForBullet);
	BulletActor->SetBulletData(Bullet);
	BulletActor->SetOffsetTimeline();
	BulletActor->SetBulletScale(HoldBulletDivideCounter);
}

void UBulletComponent::SpawnImmediatelyBombBullet()
{
	ABullet* BulletActor = GetWorld()->SpawnActor<ABullet>(Bullet.BulletClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	if (BulletActor == nullptr) return;

	BulletActor->SetBulletData(Bullet);
	BulletActor->CallExplodeBulletBomb();
}
#pragma endregion

#pragma region ////////////// BULLET AS LASER //////////////
void UBulletComponent::LaserDamageRaycast(FVector StartLocation, FVector DirForLaser)
{
	TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> Hits;
	if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), StartLocation, StartLocation + DirForLaser * Bullet.Distance, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),
		false, ActorsToIgnore, EDrawDebugTrace::None, Hits, true) == false) return;

	for (FHitResult& Hit : Hits)
	{
		ITakeDamageInterface* DamageInterface = Cast<ITakeDamageInterface>(Hit.GetActor());
		if (DamageInterface == nullptr)  continue;

		DamageInterface->TakeDamage(Bullet.Damage, Bullet.Impulse, DirForLaser);
	}
}

void UBulletComponent::ShootBulletAsLaser(const FVector& BulletSpawnLocation, const FVector& BulletShootDirection)
{
	LaserDamageRaycast(BulletSpawnLocation, BulletShootDirection);
	if (bIsLaserActive == true) return;

	if (EnemyOwner) EnemyOwner->StopEnemyMovement(Bullet.LaserTime);
	if (Bullet.LaserNiagaraParticle)
	{
		SpawnedLaserParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(Bullet.LaserNiagaraParticle, GetOwner()->GetRootComponent(), FName(""), FVector(0.f, 0.f, 0.f), FRotator(-90.f, PawnRotation.Yaw, 0.f), EAttachLocation::KeepRelativeOffset, true);
		SpawnedLaserParticle->SetWorldRotation(FRotator(-90.f, PawnRotation.Yaw, 0.f)); // There is an error whereby particles do not rotate after calling SpawnSystemAttached.
	}
	bIsLaserActive = true;
}

void UBulletComponent::TurnOffLaserAfterTime(float Delta)
{
	if (bIsLaserActive == false) return;

	CalculateAndSpawnBullets(PawnRotation);

	LaserTimeElapsed += Delta;
	if (LaserTimeElapsed > Bullet.LaserTime)
	{
		TurnOffLaser();
	}
}

void UBulletComponent::TurnOffLaser()
{
	LaserTimeElapsed = 0.f;
	bIsLaserActive = false;
	if (SpawnedLaserParticle) SpawnedLaserParticle->DestroyComponent();
}
#pragma endregion

#pragma region /////////// HOLD BULLET /////////////
bool UBulletComponent::WasHoldBulletActivated()
{
	if (Bullet.bHoldBullet == true && bShouldSkipHoldBullet == false)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(HoldBulletHandle, this, &UBulletComponent::HoldBulletSetDamage, Bullet.HoldBulletTime * 1 / 10.f, true);
		}
		return true;
	}
	bShouldSkipHoldBullet = false;
	return false;
}

bool UBulletComponent::ShouldSkipHoldBullet()
{
	return Bullet.bHoldBullet == true && GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle);
}

void UBulletComponent::HoldBulletSetDamage()
{
	if (HoldBulletDivideCounter > 10) return;
	Bullet.Damage += DamageBeforeHoldBullet * 1 / 10.f;

	if (BaseTriangleDynamicMat)
	{
		FLinearColor NewColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Green, Bullet.HoldBulletTriangleColor, HoldBulletDivideCounter / 10.f);
		BaseTriangleDynamicMat->SetVectorParameterValue(FName("Color"), NewColor);
	}

	HoldBulletDivideCounter++;
}

void UBulletComponent::TurnOffHoldBullet()
{
	if (Bullet.bHoldBullet == false) return;

	GetWorld()->GetTimerManager().ClearTimer(HoldBulletHandle);
	Bullet.Damage = DamageBeforeHoldBullet;
	HoldBulletDivideCounter = 1;

	if (BaseTriangleDynamicMat) BaseTriangleDynamicMat->SetVectorParameterValue(FName("Color"), FLinearColor::Green);
}
#pragma endregion

void UBulletComponent::SetPlayerVariables(UMaterialInstanceDynamic* DynamicMat)
{
	TrianglePawnPlayer = Cast<ATrianglePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (TrianglePawnPlayer == nullptr) return;

	BaseTriangleDynamicMat = DynamicMat;
}
