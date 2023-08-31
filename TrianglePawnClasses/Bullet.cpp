// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/Bullet.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "TheBindingOfTriangle/Interfaces/TakeDamageInterface.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BulletMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Mesh Component"));
	RootComponent = BulletMeshComp;
	BulletMeshComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	BulletMeshComp->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementBullet(DeltaTime);	
	if (BulletData.bUseWayCurve == true)  CurveTimeline.TickTimeline(DeltaTime);
}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (BulletData.TypeOfBullet == ETB_Bomb) {
		HitAsBombBullet();
		return;
	}

	ITakeDamageInterface* TakeDamageInter = Cast<ITakeDamageInterface>(OtherActor);
	if (TakeDamageInter)
	{
		TakeDamageInter->TakeDamage(BulletData.Damage, BulletData.Impulse, TrajectoryBullet);
	}
	else if (Hit.GetComponent()->IsSimulatingPhysics() == true)
	{
		FVector ImpulseForce = TrajectoryBullet * BulletData.Impulse;
		Hit.GetComponent()->AddImpulse(ImpulseForce, NAME_None, true);
	}

	Destroy();
}

void ABullet::HitAsBombBullet()
{
	FTransform NewTransform = FTransform(FRotator(0.f), GetActorLocation(), FVector(BulletData.ExplosionParticleScale));
	if (BulletData.ExplosionParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletData.ExplosionParticle, NewTransform);

	TArray<FHitResult> HitsResultArray;
	FCollisionShape SphereCol = FCollisionShape::MakeSphere(BulletData.ExplodeRadius);
	if (GetWorld()->SweepMultiByChannel(HitsResultArray, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_Visibility, SphereCol) == false) return;

	for (FHitResult Hit : HitsResultArray)
	{
		if (Hit.GetActor() == nullptr) continue;

		ITakeDamageInterface* TakeDamageInterface = Cast<ITakeDamageInterface>(Hit.GetActor());
		if (TakeDamageInterface)
		{
			FVector Dir = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Hit.GetActor()->GetActorLocation()).Vector();
			TakeDamageInterface->TakeDamage(BulletData.Damage, BulletData.Impulse * 10.f, Dir);
		}
		else if (Hit.GetComponent()->IsSimulatingPhysics() == true)
		{
			Hit.GetComponent()->AddRadialImpulse(GetActorLocation(), BulletData.ExplodeRadius, BulletData.Impulse * 10.f, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}

	Destroy();
}

#pragma region //////////////// OFFSET TIMELINE ////////////////////
void ABullet::SetOffsetTimeline()
{
	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("OffsetTimelineProgress"));
	CurveTimeline.AddInterpFloat(BulletData.WayCurve, TimelineProgress);
	CurveTimeline.SetLooping(true);

	CurveTimeline.PlayFromStart();
}

void ABullet::OffsetTimelineProgress(float Value)
{
	CurveOffset = Value;
}

void ABullet::ShouldUseCurveOffset(FVector& LocAfterOffset)
{
	if (BulletData.bUseWayCurve == false) return;

	FVector RightVec = FVector::CrossProduct(TrajectoryBullet, GetActorUpVector());
	LocAfterOffset += RightVec * CurveOffset;
}
#pragma endregion

#pragma region /////////////////// MOVEMENT ////////////////////////
void ABullet::MovementBullet(float Delta)
{
	FVector NextLoc = GetActorLocation() + (TrajectoryBullet * BulletData.Speed * Delta);

	if (TrackBulletDistance >= BulletData.Distance)
	{
		if (ShouldBulletGoBack() == true) return;
		NextLoc -= GetActorUpVector() * (BulletFallingDown * Delta);
		BulletFallingDown += 20 * Delta;

		SetScaleWhileDroppingDown(Delta);
	}
	else
	{
		TrackBulletDistance += (BulletData.Speed * Delta);
	}

	ShouldUseCurveOffset(NextLoc);

	SetActorLocation(NextLoc, true, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

void ABullet::SetScaleWhileDroppingDown(float Delta)
{
	FVector nextScale = BulletMeshComp->GetRelativeScale3D() - (0.25 * Delta);
	float minScale = FMath::Clamp(nextScale.X, 0.005f, BulletScale.X);

	BulletMeshComp->SetRelativeScale3D(FVector(minScale));
}

bool ABullet::ShouldBulletGoBack()
{
	if (BulletData.bShouldBack == false) return false;

	TrackBulletDistance = 0.f;
	TrajectoryBullet *= -1;
	BulletData.bShouldBack = false;
	return true;
}
#pragma endregion


void ABullet::SetBulletScale(int32 NewCounter)
{
	if (BulletData.bHoldBullet == true)
	{
		BulletScale = BulletMeshComp->GetRelativeScale3D() + BulletMeshComp->GetRelativeScale3D() * (NewCounter / 10.f);
		BulletMeshComp->SetRelativeScale3D(BulletScale);
	}
	else BulletScale = BulletMeshComp->GetRelativeScale3D();
}


