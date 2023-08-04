// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TrianglePawn.generated.h"

UCLASS()
class THEBINDINGOFTRIANGLE_API ATrianglePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATrianglePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* TriangleCapsuleComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* TriangleMeshComp;
	UPROPERTY(EditAnywhere, Category = "Components")
		class ACameraActor* TriangleCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float MovementForce = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float CounterMovementForce = 3000.f;

	// Movement
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	FVector CounterMovement();

	void Shoot_Right(float Axis);
	void Shoot_Forward(float Axis);


	FRotator* CurrentRotation;

};
