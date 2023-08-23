// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/EnemyClasses/BaseEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box Component"));
	RootComponent = CollisionBoxComp;
	CollisionBoxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Pawn Movement"));

	EnemyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy Mesh Component"));
	EnemyMeshComp->SetupAttachment(CollisionBoxComp);
	EnemyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BulletComp = CreateDefaultSubobject<UBulletComponent>(TEXT("Bullet Component"));

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

