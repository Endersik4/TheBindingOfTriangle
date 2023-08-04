// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

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
	FVector Force = Axis * GetActorForwardVector() * MovementForce * 10 + CounterMovement();
	TriangleCapsuleComp->AddForce(Force);
}

void ATrianglePawn::MoveRight(float Axis)
{
	FVector Force = Axis * GetActorRightVector() * MovementForce * 10 + CounterMovement();
	TriangleCapsuleComp->AddForce(Force);
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
	if (Axis > 0.5f)  CurrentRotation->Yaw = 90.f;
	else if (Axis < -0.5f)  CurrentRotation->Yaw = -90.f;
}

void ATrianglePawn::Shoot_Forward(float Axis)
{
	if (Axis > 0.5f) CurrentRotation->Yaw = 0.f;
	else if (Axis < -0.5f)  CurrentRotation->Yaw = -180.f;
}

