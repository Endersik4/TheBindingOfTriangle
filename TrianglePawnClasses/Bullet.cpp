// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/Bullet.h"
#include "Components/StaticMeshComponent.h"

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

	BulletScale = BulletMeshComp->GetRelativeScale3D();
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
	FVector NextLoc = GetActorLocation() + (TrajectoryBullet * BulletData.Speed * Delta) * BulletGoBack;

	if (TrackBulletDistance >= BulletData.Distance)
	{
		if (ShouldBulletGoBack() == true) return;
		NextLoc -= GetActorUpVector() * (BulletFallingDown * Delta);
		BulletFallingDown += 20 * Delta;

		SetScaleWhileDroppingDown(Delta);
	}
	else
	{
		TrackBulletDistance += (BulletData.Speed * Delta) * BulletGoBack;
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

	BulletGoBack = -1;
	TrackBulletDistance = 0.f;
	BulletData.bShouldBack = false;
	return true;
}
#pragma endregion

