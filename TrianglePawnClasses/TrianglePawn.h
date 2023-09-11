// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TheBindingOfTriangle/Interfaces/TakeDamageInterface.h"

#include "TrianglePawn.generated.h"

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
		void SetCanGetHit(bool bCan) { bCanGetHit = bCan; }

	int32 GetCoinsAmount() const { return CoinsAmount; }
	int32 GetBombsAmount() const { return BombsAmount; }
	int32 GetKeysAmount() const { return KeysAmount; }

	bool AddCoins(int32 AmountToAdd);
	bool AddBombs(int32 AmountToAdd);
	bool AddKeys(int32 AmountToAdd);
	bool AddHearts(int32 AmountToAdd, FString HeartName);
	void AddSlotForHeart(int32 SlotsToAdd, FString HeartName);

	void ChangeCameraRoom(bool bChangeLoc, FVector CameraLocation);

	void SpawnItemDescriptionWidget(FText Description);

	void RestartHudWidgetVariables();

	class UStaticMeshComponent* GetTriangleMeshComp() const { return TriangleMeshComp; }

	UPROPERTY(EditAnywhere, Category = "Components")
		class ACameraActor* TriangleCamera;
	UPROPERTY(Instanced, EditAnywhere, BlueprintReadWrite, Category = "Components")
		class UBulletComponent* BulletComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* TriangleBoxComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* TriangleMeshComp;

	UPROPERTY(EditAnywhere, Category = "Player Settings")
		int32 MaxOfAllHearts = 40;
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
	UPROPERTY(EditAnywhere, Category = "Player Settings|Widgets")
		TSubclassOf<class UAwardDescriptionWidget> ItemDescriptionWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Room Settings|Camera")
		float CameraChangeLocationTime = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float MovementForce = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement Settings")
		float CounterMovementForce = 3000.f;

	// Movement
	void MoveForward(float Axis);
	void MoveRight(float Axis);
	FVector CounterMovement();

	// Shoot 
	bool CanShoot(float Axis);
	void Shoot(float Axis);
	void Shoot_Right(float Axis);
	void Shoot_Forward(float Axis);

	// Damage
	bool bCanGetHit = true;
	UMaterialInstanceDynamic* BaseTriangleDynamicMat;

	// Place Bomb
	void PlaceBomb();

	// Add Item
	bool AddAmount(int32& Value, int32 AmountToAdd);

	// Widgets
	class UHUDWidget* HudWidget;
	void MakeHudWidget();

	// Camera
	void SetTriangleCamera();
	// Move camera
	bool bChangeCameraLocation;
	float CameraLocationTimeElapsed;
	FVector CameraStartPosition;
	FVector CameraEndPosition;
	void SmoothCameraLocation(float Delta);

	// Math
	float easeInOutCubic(float t);

	APlayerController* TrianglePawnController;
};
