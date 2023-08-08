// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/Bullet.h"


// Sets default values
ATrianglePawn::ATrianglePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriangleCapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Triangle Capsule Comp"));
	//TriangleCapsuleComp->SetConstraintMode(EDOFMode::XYPlane);
	RootComponent = TriangleCapsuleComp;

	TriangleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Triangle Mesh Comp"));
	TriangleMeshComp->SetupAttachment(RootComponent);
	TriangleMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void ATrianglePawn::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentRotation = &TriangleMeshComp->GetRelativeRotation_DirectMutable();
	if (TriangleCamera)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(TriangleCamera);
	}
}

// Called every frame
void ATrianglePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SpawnDebugBullet(DeltaTime);
}

// Called to bind functionality to input
void ATrianglePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &ATrianglePawn::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &ATrianglePawn::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Shoot_Forward"), this, &ATrianglePawn::Shoot_Forward);
	PlayerInputComponent->BindAxis(TEXT("Shoot_Right"), this, &ATrianglePawn::Shoot_Right);

}

void ATrianglePawn::MoveForward(float Axis)
{
	float Speed = MovementForce * 10 / (GetInputAxisValue(FName(TEXT("Right"))) != 0.f ? 1.3f : 1);
	FVector Force = Axis * GetActorForwardVector() * Speed + CounterMovement();
	TriangleCapsuleComp->AddImpulse(Force);
}

void ATrianglePawn::MoveRight(float Axis)
{
	float Speed = MovementForce * 10 / (GetInputAxisValue(FName(TEXT("Forward"))) != 0.f ? 1.3f : 1);
	FVector Force = Axis * GetActorRightVector() * Speed + CounterMovement();
	TriangleCapsuleComp->AddImpulse(Force);
}

FVector ATrianglePawn::CounterMovement()
{
	FVector Velocity = GetVelocity();
	Velocity.X *= -1.f;
	Velocity.Y *= -1.f;
	return FVector(CounterMovementForce * Velocity.X, CounterMovementForce * Velocity.Y, 0);
}

void ATrianglePawn::Shoot_Right(float Axis)
{
	if (Axis == 0.f) return;
	
	if (Axis > 0.5f)  TriangleMeshComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	else if (Axis < -0.5f)  TriangleMeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	SpawnDebugBullet();
	if (bCanSpawnAnotherBullet == true)
	{
		bCanSpawnAnotherBullet = false;
		GetWorldTimerManager().SetTimer(SpawnAnotherBulletHandle, this, &ATrianglePawn::SetCanSpawnAnotherBullet, Bullet.FrequencyTime, false);
	}
}

void ATrianglePawn::Shoot_Forward(float Axis)
{
	if (Axis == 0.f) return;

	if (Axis > 0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	else if (Axis < -0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, -180.f, 0.f));

	SpawnDebugBullet();
	if (bCanSpawnAnotherBullet == true)
	{
		bCanSpawnAnotherBullet = false;
		GetWorldTimerManager().SetTimer(SpawnAnotherBulletHandle, this, &ATrianglePawn::SetCanSpawnAnotherBullet, Bullet.FrequencyTime, false);
	}
}

void ATrianglePawn::SpawnDebugBullet()
{
	if (bCanSpawnAnotherBullet == false) return;
	if (bShouldDrawDebugBullets == true) UKismetSystemLibrary::DrawDebugCircle(GetWorld(), GetActorLocation(), Bullet.CirceRadius, 34, FLinearColor::White, 999.f, 1.f, FVector(0.f, 1.f, 0.f), FVector(1.0f, 0.f, 0.f));

	for (int i = 0; i != Bullet.Amount; i++)
	{
		float angReg = FMath::DegreesToRadians(Bullet.DegreeBetween * i + Bullet.CirceAngle + TriangleMeshComp->GetRelativeRotation().Yaw);
		FVector Line = FVector(FMath::Cos(angReg) * Bullet.CirceRadius, FMath::Sin(angReg) * Bullet.CirceRadius, 0) + GetActorLocation();
		FVector Trajectory = FVector(FMath::Cos(angReg), FMath::Sin(angReg), 0);
		
		if (bShouldDrawDebugBullets == true)
		{
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Line, 5.f, 6, FLinearColor::Red, 999.f, 2.f);
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), Line, Line + Trajectory * 100.f, FLinearColor::Red, 999.f, 1.f);
		}

		ABullet* BulletActor = GetWorld()->SpawnActor<ABullet>(BulletClass, Line, GetActorRotation());
		if (BulletActor)
		{
			BulletActor->SetTrajectoryBullet(Trajectory);
			BulletActor->SetBulletData(Bullet);
		}
	}
}

