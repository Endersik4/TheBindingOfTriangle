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

	if (bShootLaser == true)
	{
		DirectionForBullets(PawnRotation);
		LaserTimeElapsed += DeltaTime;
		if (LaserTimeElapsed > Bullet.LaserTime)
		{
			RestartLaser();
		}
	}
}

void UBulletComponent::Shoot(FRotator MeshRelativeRotation)
{
	if (bCanSpawnAnotherBullet == false) return;

	if (bShootLaser == true)
	{
		RestartLaser();
	}

	if (Bullet.bShootHoldBulletAfterTime == true && HoldBulletDivideCounter < 8 && GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle) == true)
	{
		ClearHoldBullet();
		return;
	}
	
	DirectionForBullets(MeshRelativeRotation);
	ClearHoldBullet();

	// Wait FrequencyTime to fire another bullet
	if (bCanSpawnAnotherBullet == true)
	{
		bCanSpawnAnotherBullet = false;
		GetWorld()->GetTimerManager().SetTimer(SpawnAnotherBulletHandle, this, &UBulletComponent::SetCanSpawnAnotherBullet, Bullet.FrequencyTime, false);
	}
}

void UBulletComponent::DirectionForBullets(FRotator MeshRelativeRotation)
{
	if (bShouldDrawDebugBullets == true) UKismetSystemLibrary::DrawDebugCircle(GetWorld(), GetOwner()->GetActorLocation(), Bullet.CirceRadius, 34, FLinearColor::White, 999.f, 1.f, FVector(0.f, 1.f, 0.f), FVector(1.0f, 0.f, 0.f));

	for (int i = 0; i != Bullet.Amount; i++)
	{
		// Calculate Start Location and bullet Direction
		float angReg = FMath::DegreesToRadians(Bullet.DegreeBetween * i + Bullet.CirceAngle + MeshRelativeRotation.Yaw);
		FVector BulletLocation = FVector(FMath::Cos(angReg) * Bullet.CirceRadius, FMath::Sin(angReg) * Bullet.CirceRadius, 0) + GetOwner()->GetActorLocation();
		FVector BulletTrajectory = FVector(FMath::Cos(angReg), FMath::Sin(angReg), 0);

		if (bShouldDrawDebugBullets == true)
		{
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), BulletLocation, 5.f, 6, FLinearColor::Red, 999.f, 2.f);
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), BulletLocation, BulletLocation + BulletTrajectory * 100.f, FLinearColor::Red, 999.f, 1.f);
		}

		if (Bullet.TypeOfBullet == ETB_Laser)
		{
			PawnRotation = MeshRelativeRotation;
			LaserBullet(BulletLocation, BulletTrajectory);
			if (bShootLaser == false)
			{
				if (EnemyOwner) EnemyOwner->StopEnemyMovement(Bullet.LaserTime);
				if (Bullet.LaserNiagaraParticle)
				{
					SpawnedLaserParticle = UNiagaraFunctionLibrary::SpawnSystemAttached(Bullet.LaserNiagaraParticle, GetOwner()->GetRootComponent(), FName(""), FVector(100.f,0.f,0.f), FRotator(-90.f, MeshRelativeRotation.Yaw, 0.f), EAttachLocation::KeepRelativeOffset, true);
					SpawnedLaserParticle->SetWorldRotation(FRotator(-90.f, MeshRelativeRotation.Yaw, 0.f));
				}
				bShootLaser = true;
			}
		}	
		else SpawnBullet(BulletLocation, BulletTrajectory);
	}
}

void UBulletComponent::SpawnBullet(FVector StartLocation, FVector DirForBullet)
{
	ABullet* BulletActor = GetWorld()->SpawnActor<ABullet>(Bullet.BulletClass, StartLocation, GetOwner()->GetActorRotation());
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

void UBulletComponent::LaserBullet(FVector StartLocation, FVector DirForLaser)
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

void UBulletComponent::RestartLaser()
{
	LaserTimeElapsed = 0.f;
	bShootLaser = false;
	if (SpawnedLaserParticle) SpawnedLaserParticle->DestroyComponent();
}

bool UBulletComponent::HoldBullet()
{
	if (Bullet.bHoldBullet == true && bShouldSkipHoldBullet == false)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(HoldBulletHandle, this, &UBulletComponent::HoldBulletSetDamage, Bullet.HoldBulletTime * 1 / 10.f, true);
		}
		return false;
	}
	bShouldSkipHoldBullet = false;
	return true;
}

bool UBulletComponent::ShouldSkipHoldBullet()
{
	return Bullet.bHoldBullet == true && GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle);
}

void UBulletComponent::SetPlayerVariables(UMaterialInstanceDynamic* DynamicMat)
{
	TrianglePawnPlayer = Cast<ATrianglePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (TrianglePawnPlayer == nullptr) return;
	BaseTriangleDynamicMat = DynamicMat;
}

void UBulletComponent::HoldBulletSetDamage()
{
	if (HoldBulletDivideCounter > 10) return;
	UE_LOG(LogTemp, Warning, TEXT("COUNTER %d"), HoldBulletDivideCounter);
	Bullet.Damage += DamageBeforeHoldBullet * 1 / 10.f;

	if (BaseTriangleDynamicMat)
	{
		FLinearColor NewColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Green, Bullet.HoldBulletTriangleColor, HoldBulletDivideCounter / 10.f);
		BaseTriangleDynamicMat->SetVectorParameterValue(FName("Color"), NewColor);
	}

	HoldBulletDivideCounter++;
}

void UBulletComponent::ClearHoldBullet()
{
	if (Bullet.bHoldBullet == false) return;

	GetWorld()->GetTimerManager().ClearTimer(HoldBulletHandle);
	Bullet.Damage = DamageBeforeHoldBullet;
	HoldBulletDivideCounter = 1;

	if (BaseTriangleDynamicMat) BaseTriangleDynamicMat->SetVectorParameterValue(FName("Color"), FLinearColor::Green);
}

