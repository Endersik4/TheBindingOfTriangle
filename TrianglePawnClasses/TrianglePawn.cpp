// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

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
}

void ATrianglePawn::Shoot_Forward(float Axis)
{
	if (Axis == 0.f) return;

	if (Axis > 0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	else if (Axis < -0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, -180.f, 0.f));
}

void ATrianglePawn::SpawnDebugBullet()
{
	UE_LOG(LogTemp, Warning, TEXT("DZIALA"));
	UKismetSystemLibrary::DrawDebugCircle(GetWorld(), GetActorLocation(), Bullet.CirceRadius, 12, FLinearColor::White, 0.f, 1.f);
	float angReg = Bullet.DegreeBetween;
	FMath::DegreesToRadians(angReg);
	FVector v = FVector(FMath::Cos(angReg), FMath::Sin(angReg), GetActorLocation().Z);
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetActorLocation(), v, 2.f, FLinearColor::Red, 0.f, 2.f);
	//FVector2D StartingVector = FVector2D(GetActorLocation() + GetActorForwardVector() * Bullet.CirceRadius);

	/*for (int i = 0; i != Bullet.Amount; i++)
	{
		UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetActorLocation(), StartingVector )
	}*/
}

