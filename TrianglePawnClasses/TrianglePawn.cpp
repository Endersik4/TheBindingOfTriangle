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
#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"

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

	BulletComponent = CreateDefaultSubobject<UBulletComponent>(TEXT("Bullet Component"));


}

// Called when the game starts or when spawned
void ATrianglePawn::BeginPlay()
{
	Super::BeginPlay();

	SetTriangleCamera();
	MakeHudWidget();

	BaseTriangleDynamicMat = UMaterialInstanceDynamic::Create(TriangleMeshComp->GetMaterial(1), this);
	TriangleMeshComp->SetMaterial(1, BaseTriangleDynamicMat);
	BulletComponent->SetPlayerVariables(BaseTriangleDynamicMat);
}

// Called every frame
void ATrianglePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SmoothCameraLocation(DeltaTime);
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
	BulletComponent->RestartLaser();

	if (BulletComponent->HoldBullet() == false) return;

	BulletComponent->Shoot(TriangleMeshComp->GetRelativeRotation());
}

void ATrianglePawn::Shoot_Forward(float Axis)
{
	if (Axis > 0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	else if (Axis < -0.5f) TriangleMeshComp->SetRelativeRotation(FRotator(0.f, -180.f, 0.f));
	
	if (CanShoot(Axis) == false) return;
	BulletComponent->RestartLaser();

	if (BulletComponent->HoldBullet() == false) return;

	BulletComponent->Shoot(TriangleMeshComp->GetRelativeRotation());
}

bool ATrianglePawn::CanShoot(float Axis)
{
	if (Axis == 0.f)
	{
		if (BulletComponent->ShouldSkipHoldBullet() == true
			&& GetInputAxisValue(FName(TEXT("Shoot_Forward"))) == 0.f && GetInputAxisValue(FName(TEXT("Shoot_Right"))) == 0.f)
		{
			BulletComponent->SetShouldSkipHoldBullet(true);
		}
		else return false;
	}

	return true;
}

#pragma endregion

#pragma region /////////////// TAKE DAMAGE /////////////////

void ATrianglePawn::TakeDamage(float Damage, float Impulse, FVector ImpulseDir)
{
	if (bCanGetHit == false) return;

	CurrentHearts.Last().Amount -= FMath::TruncToInt(Damage);
	if (CurrentHearts.Last().Amount <= 0)
	{
		if (CurrentHearts.Num() != 1) CurrentHearts.RemoveAt(CurrentHearts.Num() - 1);
		else UE_LOG(LogTemp, Error, TEXT("DEAD"));	
	}
	CurrentHearts.Sort();
	RestartHudWidgetVariables();

	TriangleBoxComp->AddImpulse(ImpulseDir * Impulse, NAME_None, true);
	
	bCanGetHit = false;
	ChangeColorAfterHit(BaseTriangleDynamicMat);
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
	if (Value < 0) Value = 0;

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

#pragma region ////////////////// CAMERA /////////////////////
void ATrianglePawn::ChangeCameraRoom(bool bChangeLoc, FVector CameraLocation)
{
	bChangeCameraLocation = bChangeLoc;
	CameraStartPosition = TriangleCamera->GetActorLocation();
	CameraEndPosition = CameraLocation;
}

void ATrianglePawn::SmoothCameraLocation(float Delta)
{
	if (bChangeCameraLocation == false) return;

	CameraLocationTimeElapsed += Delta;
	float t = FMath::Clamp(CameraLocationTimeElapsed / CameraChangeLocationTime, 0, 1);

	t = easeInOutCubic(t);

	FVector NewLocation = FMath::Lerp(CameraStartPosition, CameraEndPosition, t);
	TriangleCamera->SetActorLocation(NewLocation);

	if (t >= 1.f)
	{
		bChangeCameraLocation = false;
		CameraLocationTimeElapsed = 0.f;
	}
}
#pragma endregion

float ATrianglePawn::easeInOutCubic(float t)
{
	return t < 0.5 ? 4 * FMath::Pow(t, 3) : 1 - FMath::Pow(-2 * t + 2, 3) / 2;
}