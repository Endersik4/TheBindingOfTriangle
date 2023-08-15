// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Items/ExplosiveBomb.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PhysicsVolume.h"

// Sets default values
AExplosiveBomb::AExplosiveBomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bomb Mesh"));
	RootComponent = BombMesh;

	BombMesh->SetCollisionProfileName(FName(TEXT("PhysicsActor")));
	//BombMesh->SetSimulatePhysics(true);
	BombMesh->GetBodyInstance()->bLockXRotation = true;
	BombMesh->GetBodyInstance()->bLockYRotation = true;
	BombMesh->GetBodyInstance()->bLockZRotation = true;
	BombMesh->GetBodyInstance()->bLockZTranslation = true;
}

// Called when the game starts or when spawned
void AExplosiveBomb::BeginPlay()
{
	Super::BeginPlay();

	BombColorDynamic = UMaterialInstanceDynamic::Create(BombMesh->GetMaterial(1), this);
	BombMesh->SetMaterial(1, BombColorDynamic);

	SetExplodeTimeline();
	GetWorld()->GetTimerManager().SetTimer(ExplodeHandle, this, &AExplosiveBomb::Explode, TimeToExplode, false);
}

// Called every frame
void AExplosiveBomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ExplodeCurveTimeline.TickTimeline(DeltaTime);
}

#pragma region /////////////////// EXPLODE - COLOR TIMELINE /////////////
void AExplosiveBomb::SetExplodeTimeline()
{
	if (ColorChangeExplodingCurve == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("COLOR CHANGE CURVE = NONE in EXPLOSIVE BOMB"));
		return;
	}

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindUFunction(this, FName("ExplodeTimelineProgress"));
	ExplodeCurveTimeline.SetTimelineLength(TimeToExplode);
	ExplodeCurveTimeline.AddInterpFloat(ColorChangeExplodingCurve, TimelineProgress);

	ExplodeCurveTimeline.PlayFromStart();
}

void AExplosiveBomb::ExplodeTimelineProgress(float Value)
{
	BombColorDynamic->SetScalarParameterValue(FName(TEXT("LerpValue")), Value);

	if (ScaleChangeExplodingCurve == nullptr) return;
	FVector NewScale = BombMesh->GetRelativeScale3D() + FVector(ScaleChangeExplodingCurve->GetFloatValue(ExplodeCurveTimeline.GetPlaybackPosition()));
	BombMesh->SetRelativeScale3D(NewScale);
}
#pragma endregion

void AExplosiveBomb::Explode()
{
	Destroy();
}

