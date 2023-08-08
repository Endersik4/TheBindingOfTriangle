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
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MovementBullet(DeltaTime);
}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Destroy();
}

void ABullet::MovementBullet(float Delta)
{
	FVector NextLoc = GetActorLocation() + TrajectoryBullet * BulletData.Speed * Delta;
	SetActorLocation(NextLoc, true, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
}

