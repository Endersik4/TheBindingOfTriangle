// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBomb.generated.h"

UCLASS()
class THEBINDINGOFTRIANGLE_API AExplosiveBomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* BombMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		float TimeToExplode;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		float RadiusDistance;

	FTimerHandle ExplodeHandle;
	void Explode();

};
