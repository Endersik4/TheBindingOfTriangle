// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
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

	UFUNCTION()
		void ExplodeTimelineProgress(float Value);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* BombMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		float Damage = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		float TimeToExplode = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		float ExplodeDistance = 350.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		float ExplodeImpulse = 250.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings")
		bool bDrawDebugExplodeSphere;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings|Explosion")
		UParticleSystem* ExplosionParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings|Explosion")
		float ExplosionParticleScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings|Explosion")
		UCurveFloat* ColorChangeExplodingCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Bomb Settings|Explosion")
		UCurveFloat* ScaleChangeExplodingCurve;

	void SetExplodeTimeline();
	FTimeline ExplodeCurveTimeline;

	UMaterialInstanceDynamic* BombColorDynamic;

	void ExplodeObjectsInRange();

	FTimerHandle ExplodeHandle;
	void Explode();

};
