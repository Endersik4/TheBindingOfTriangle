// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/Bullet.h"
#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"

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

	// ...
}

void UBulletComponent::Shoot(FRotator MeshRelativeRotation)
{
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
	if (bCanSpawnAnotherBullet == false) return;
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

		SpawnBullet(BulletLocation, BulletTrajectory);
	}
}

void UBulletComponent::SpawnBullet(FVector StartLocation, FVector DirForBullet)
{
	ABullet* BulletActor = GetWorld()->SpawnActor<ABullet>(BulletClass, StartLocation, GetOwner()->GetActorRotation());
	if (BulletActor == nullptr) return;

	BulletActor->SetTrajectoryBullet(DirForBullet);
	BulletActor->SetBulletData(Bullet);
	BulletActor->SetOffsetTimeline();
	BulletActor->SetBulletScale(HoldBulletDivideCounter);
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
	Bullet.Damage += DamageBeforeHoldBullet * 1 / 10.f;

	if (BaseTriangleDynamicMat)
	{
		FLinearColor NewColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Green, FLinearColor::Yellow, HoldBulletDivideCounter / 10.f);
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

