// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TheBindingOfTriangle/Interfaces/TakeDamageInterface.h"

#include "TrianglePawn.generated.h"

USTRUCT(BlueprintType)
struct FBulletStruct {

	GENERATED_USTRUCT_BODY();

	// After Distance bullet will be destroyed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Damage = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Distance = 900.f;
	// How fast bullet will go 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Speed = 1400.f;
	// How many bullets will be spawned that is facing the same directions after given time (FrequencyTime)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float FrequencyTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		float Impulse = 200.f;
	// Should use Way Curve when spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bUseWayCurve = false;
	// Curve for relative additional movement to the bullet, for example: instead of going straight line bullet can waving 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (EditCondition = "bUseWayCurve", EditConditionHides))
		UCurveFloat* WayCurve = nullptr;
	// Should Bullet back once the distance is reached and after this reach the distance again 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bShouldBack = false;
	// Longer button presses cause more bullet damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings")
		bool bHoldBullet = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (EditCondition = "bHoldBullet", EditConditionHides))
		float HoldBulletTime = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (EditCondition = "bHoldBullet", EditConditionHides))
		FLinearColor HoldBulletTriangleColor = FLinearColor::Yellow;
	// How many bullets can be spawned at the same time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once")
		int32 Amount = 1;
	// Angle between bullets when there is more then 1 bullet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0"))
		float DegreeBetween = 45.f;
	// Radius of Circe that bullets will be spawned on the edges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once")
		float CirceRadius = 100.f;
	// How much rotate circe around player 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings|Many Bullet At Once")
		float CirceAngle = 0.f;
};

USTRUCT(BlueprintType)
struct FHeartStruct {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
		FString HeartName;
	// 1 point of Health = Half of Heart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
		int32 Amount;
	// How should the hearts be ordered in the TileView on the HUD Widget (1 = first, 2 - second, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
		int32 HeartOrder = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
		UTexture2D* WholeHeartTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
		UTexture2D* HalfHeartTexture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings")
		bool bEmptyTextureAfterDelete = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings", meta = (EditCondition = "bEmptyTextureAfterDelete", EditConditionHides))
		int32 MaxAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heart Settings", meta = (EditCondition = "bEmptyTextureAfterDelete", EditConditionHides))
		UTexture2D* EmptyHeartTexture;

	FHeartStruct()
	{
		HeartName = "";
		Amount = 0;
		HeartOrder = 1;
		WholeHeartTexture = nullptr;
		HalfHeartTexture = nullptr;
		bEmptyTextureAfterDelete = false;
		MaxAmount = 0;
		EmptyHeartTexture = nullptr;
	}
public:

	bool operator<(const FHeartStruct& Heart) const { return HeartOrder < Heart.HeartOrder; }
	bool operator>(const FHeartStruct& Heart) const { return HeartOrder > Heart.HeartOrder; }
};

UCLASS()
class THEBINDINGOFTRIANGLE_API ATrianglePawn : public APawn, public ITakeDamageInterface
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

	// Take Damage Interface
	virtual void TakeDamage(float Damage, float Impulse, FVector ImpulseDir) override;

	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
		void ChangeColorAfterHit(UMaterialInstanceDynamic* TriangleMaterial);

	UFUNCTION(BlueprintCallable)
		void DirectionForBullets();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Settings")
		bool bShouldDrawDebugBullets = false;

	UFUNCTION(BlueprintCallable)
		FBulletStruct GetBulletData() const { return Bullet; }

	int32 GetCoinsAmount() const { return CoinsAmount; }
	int32 GetBombsAmount() const { return BombsAmount; }
	int32 GetKeysAmount() const { return KeysAmount; }

	bool AddCoins(int32 AmountToAdd);
	bool AddBombs(int32 AmountToAdd);
	bool AddKeys(int32 AmountToAdd);
	bool AddHearts(int32 AmountToAdd, FString HeartName);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* TriangleBoxComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* TriangleMeshComp;
	UPROPERTY(EditAnywhere, Category = "Components")
		class ACameraActor* TriangleCamera;

	UPROPERTY(EditAnywhere, Category = "Player Settings")
		TArray<FHeartStruct> CurrentHearts;
	UPROPERTY(EditAnywhere, Category = "Player Settings")
		TSubclassOf<class AExplosiveBomb> ExplosiveBombClass;
	UPROPERTY(EditAnywhere, Category = "Player Settings|Inventory", meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
		int32 CoinsAmount;
	UPROPERTY(EditAnywhere, Category = "Player Settings|Inventory", meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
		int32 BombsAmount = 1;
	UPROPERTY(EditAnywhere, Category = "Player Settings|Inventory", meta = (ClampMin = "0", ClampMax = "99", UIMin = "0", UIMax = "99"))
		int32 KeysAmount;
	UPROPERTY(EditAnywhere, Category = "Player Settings|Widgets")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float MovementForce = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float CounterMovementForce = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		TSubclassOf<class ABullet> BulletClass;
	UPROPERTY(EditAnywhere, Category = "Bullet Settings")
		FBulletStruct Bullet;

	// Movement
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	FVector CounterMovement();

	// Shoot 
	bool CanShoot(float Axis);
	void Shoot_Right(float Axis);
	void Shoot_Forward(float Axis);
	void Shoot();

	// Damage
	UMaterialInstanceDynamic* BaseTriangleDynamicMat;

	// Bullet
	void SpawnBullet(FVector StartLocation, FVector DirForBullet);

	// Frequency Bullet
	bool bCanSpawnAnotherBullet = true;
	FTimerHandle SpawnAnotherBulletHandle;
	void SetCanSpawnAnotherBullet() { bCanSpawnAnotherBullet = true; }

	// Place Bomb
	void PlaceBomb();
	
	// Hold Bullet
	FTimerHandle HoldBulletHandle;
	float DamageBeforeHoldBullet;
	int32 HoldBulletDivideCounter = 1;
	bool bShouldSkipHoldBullet = false;
	bool HoldBullet();
	void HoldBulletSetDamage();
	void ClearHoldBullet();

	// Add Item
	bool AddAmount(int32& Value, int32 AmountToAdd);

	// Widgets
	class UHUDWidget* HudWidget;
	void MakeHudWidget();
	void RestartHudWidgetVariables();

	// Camera
	void SetTriangleCamera();

};
