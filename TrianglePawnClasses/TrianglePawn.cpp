// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/Bullet.h"
#include "TheBindingOfTriangle/Widgets/HUDWidget.h"
#include "TheBindingOfTriangle/Interfaces/TakeItemInterface.h"
#include "TheBindingOfTriangle/Items/ExplosiveBomb.h"

// Sets default values
ATrianglePawn::ATrianglePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriangleBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Triangle Box Comp"));
	RootComponent = TriangleBoxComp;

	TriangleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Triangle Mesh Comp"));
	TriangleMeshComp->SetupAttachment(TriangleBoxComp);
	TriangleMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriangleBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATrianglePawn::OnComponentBeginOverlap);
	TriangleBoxComp->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

}

// Called when the game starts or when spawned
void ATrianglePawn::BeginPlay()
{
	Super::BeginPlay();
	
	DamageBeforeHoldBullet = Bullet.Damage;

	SetTriangleCamera();
	MakeHudWidget();

	BaseTriangleDynamicMat = UMaterialInstanceDynamic::Create(TriangleMeshComp->GetMaterial(1), this);
	TriangleMeshComp->SetMaterial(1, BaseTriangleDynamicMat);
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

	PlayerInputComponent->BindAction(TEXT("PlaceBomb"), IE_Pressed, this, &ATrianglePawn::PlaceBomb);

}

#pragma region //////////////// MOVEMENT ///////////////////

void ATrianglePawn::MoveForward(float Axis)
{
	float Speed = MovementForce * 10 / (GetInputAxisValue(FName(TEXT("Right"))) != 0.f ? 1.3f : 1);
	FVector Force = Axis * GetActorForwardVector() * Speed + CounterMovement();
	TriangleBoxComp->AddImpulse(Force);
}

void ATrianglePawn::MoveRight(float Axis)
{
	float Speed = MovementForce * 10 / (GetInputAxisValue(FName(TEXT("Forward"))) != 0.f ? 1.3f : 1);
	FVector Force = Axis * GetActorRightVector() * Speed + CounterMovement();
	TriangleBoxComp->AddImpulse(Force);
}

FVector ATrianglePawn::CounterMovement()
{
	FVector Velocity = GetVelocity();
	Velocity.X *= -1.f;
	Velocity.Y *= -1.f;
	return FVector(CounterMovementForce * Velocity.X, CounterMovementForce * Velocity.Y, 0);
}

#pragma endregion

#pragma region ////////////////// SHOOT ////////////////////
void ATrianglePawn::Shoot_Right(float Axis)
{
	if (Axis > 0.5f)  TriangleMeshComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	else if (Axis < -0.5f)  TriangleMeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	if (CanShoot(Axis) == false) return;

	if (HoldBullet() == false) return;

	Shoot();
}

void ATrianglePawn::Shoot_Forward(float Axis)
{
	if (Axis > 0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	else if (Axis < -0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, -180.f, 0.f));

	if (CanShoot(Axis) == false) return;

	if (HoldBullet() == false) return;

	Shoot();
}

void ATrianglePawn::Shoot()
{
	DirectionForBullets();
	ClearHoldBullet();

	// Wait FrequencyTime to fire another bullet
	if (bCanSpawnAnotherBullet == true)
	{
		bCanSpawnAnotherBullet = false;
		GetWorldTimerManager().SetTimer(SpawnAnotherBulletHandle, this, &ATrianglePawn::SetCanSpawnAnotherBullet, Bullet.FrequencyTime, false);
	}
}

bool ATrianglePawn::CanShoot(float Axis)
{
	if (Axis == 0.f)
	{
		if (Bullet.bHoldBullet == true && GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle) == true 
			&& GetInputAxisValue(FName(TEXT("Shoot_Forward"))) == 0.f && GetInputAxisValue(FName(TEXT("Shoot_Right"))) == 0.f)
		{
			bShouldSkipHoldBullet = true;
		}
		else return false;
	}

	return true;
}

#pragma endregion

#pragma region /////////////// TAKE DAMAGE /////////////////

void ATrianglePawn::TakeDamage(float Damage, float Impulse, FVector ImpulseDir)
{
	///CurrentHealth -= FMath::TruncToInt(Damage);
	CurrentHearts.Last().Amount -= FMath::TruncToInt(Damage);
	if (CurrentHearts.Last().Amount <= 0)
	{
		if (CurrentHearts.Num() != 1) CurrentHearts.RemoveAt(CurrentHearts.Num() - 1);
		else UE_LOG(LogTemp, Error, TEXT("DEAD"));	
	}
	CurrentHearts.Sort();
	RestartHudWidgetVariables();
	
	ChangeColorAfterHit(BaseTriangleDynamicMat);
}

#pragma endregion

#pragma region ////////////////// BULLET ///////////////////
void ATrianglePawn::DirectionForBullets()
{
	if (bCanSpawnAnotherBullet == false) return;
	if (bShouldDrawDebugBullets == true) UKismetSystemLibrary::DrawDebugCircle(GetWorld(), GetActorLocation(), Bullet.CirceRadius, 34, FLinearColor::White, 999.f, 1.f, FVector(0.f, 1.f, 0.f), FVector(1.0f, 0.f, 0.f));

	for (int i = 0; i != Bullet.Amount; i++)
	{
		// Calculate Start Location and bullet Direction
		float angReg = FMath::DegreesToRadians(Bullet.DegreeBetween * i + Bullet.CirceAngle + TriangleMeshComp->GetRelativeRotation().Yaw);
		FVector BulletLocation = FVector(FMath::Cos(angReg) * Bullet.CirceRadius, FMath::Sin(angReg) * Bullet.CirceRadius, 0) + GetActorLocation();
		FVector BulletTrajectory = FVector(FMath::Cos(angReg), FMath::Sin(angReg), 0);
		
		if (bShouldDrawDebugBullets == true)
		{
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), BulletLocation, 5.f, 6, FLinearColor::Red, 999.f, 2.f);
			UKismetSystemLibrary::DrawDebugLine(GetWorld(), BulletLocation, BulletLocation + BulletTrajectory * 100.f, FLinearColor::Red, 999.f, 1.f);
		}

		SpawnBullet(BulletLocation, BulletTrajectory);
	}
}

void ATrianglePawn::SpawnBullet(FVector StartLocation, FVector DirForBullet)
{
	ABullet* BulletActor = GetWorld()->SpawnActor<ABullet>(BulletClass, StartLocation, GetActorRotation());
	if (BulletActor == nullptr) return;

	BulletActor->SetTrajectoryBullet(DirForBullet);
	BulletActor->SetBulletData(Bullet);
	BulletActor->SetOffsetTimeline();
	BulletActor->SetBulletScale(HoldBulletDivideCounter);
}
#pragma endregion

#pragma region ////////////////// WIDGETS //////////////////
void ATrianglePawn::MakeHudWidget()
{
	APlayerController* TrianglePawnController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (HUDWidgetClass && TrianglePawnController)
	{
		HudWidget = Cast<UHUDWidget>(CreateWidget(TrianglePawnController, HUDWidgetClass));
		HudWidget->AddToViewport();
		RestartHudWidgetVariables();
	}
}

void ATrianglePawn::RestartHudWidgetVariables()
{
	if (HudWidget == nullptr) return;

	HudWidget->CurrentHearts = CurrentHearts;
	HudWidget->CallAddHeartToTile();
	HudWidget->GetItemsAmount();
}
#pragma endregion

void ATrianglePawn::PlaceBomb()
{
	if (BombsAmount <= 0) return;

	GetWorld()->SpawnActor<AExplosiveBomb>(ExplosiveBombClass, GetActorLocation(), FRotator(0.f));
	BombsAmount -= 1;
	HudWidget->GetItemsAmount();
}

#pragma region ////////////////// HOLD BULLET //////////////////
bool ATrianglePawn::HoldBullet()
{
	if (Bullet.bHoldBullet == true && bShouldSkipHoldBullet == false)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(HoldBulletHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(HoldBulletHandle, this, &ATrianglePawn::HoldBulletSetDamage, Bullet.HoldBulletTime * 1 / 10.f, true);
		}
		return false;
	}
	bShouldSkipHoldBullet = false;
	return true;
}

void ATrianglePawn::HoldBulletSetDamage()
{
	if (HoldBulletDivideCounter > 10) return;
	Bullet.Damage += DamageBeforeHoldBullet* 1/10.f;

	FLinearColor NewColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Green, FLinearColor::Yellow, HoldBulletDivideCounter / 10.f);
	BaseTriangleDynamicMat->SetVectorParameterValue(FName("Color"), NewColor);

	HoldBulletDivideCounter++;
}

void ATrianglePawn::ClearHoldBullet()
{
	if (Bullet.bHoldBullet == false) return;

	GetWorld()->GetTimerManager().ClearTimer(HoldBulletHandle);
	Bullet.Damage = DamageBeforeHoldBullet;
	HoldBulletDivideCounter = 1;
	BaseTriangleDynamicMat->SetVectorParameterValue(FName("Color"), FLinearColor::Green);
}
#pragma endregion

#pragma region /////////////////// ITEMS ///////////////////

void ATrianglePawn::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ITakeItemInterface* TakeInterface = Cast<ITakeItemInterface>(OtherActor);
	if (TakeInterface == nullptr) return;

	TakeInterface->TakeItem(this);
}

bool ATrianglePawn::AddAmount(int32& Value, int32 AmountToAdd)
{
	if (Value >= 99) return false;
	Value += AmountToAdd;
	if (Value > 99) Value = 99;

	HudWidget->GetItemsAmount();
	return true;
}

bool ATrianglePawn::AddCoins(int32 AmountToAdd)
{
	return AddAmount(CoinsAmount, AmountToAdd);
}

bool ATrianglePawn::AddBombs(int32 AmountToAdd)
{
	return AddAmount(BombsAmount, AmountToAdd);
}

bool ATrianglePawn::AddKeys(int32 AmountToAdd)
{
	return AddAmount(KeysAmount, AmountToAdd);
}
bool ATrianglePawn::AddHearts(int32 AmountToAdd, FString HeartName)
{
	for (int i = 0; i != CurrentHearts.Num(); i++)
	{
		if (CurrentHearts[i].HeartName != HeartName) continue;
		
		if (CurrentHearts[i].bEmptyTextureAfterDelete == true)
		{
			if (CurrentHearts[i].MaxAmount - AmountToAdd < CurrentHearts[i].Amount) return false;
		}

		CurrentHearts[i].Amount += AmountToAdd;

		break;
	}
	HudWidget->CurrentHearts = CurrentHearts;
	HudWidget->CallAddHeartToTile();

	return true;
}
#pragma endregion

void ATrianglePawn::SetTriangleCamera()
{
	if (TriangleCamera == nullptr) return;

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(TriangleCamera);
}