// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Items/ChestWithItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

#include "TheBindingOfTriangle/Items/PickableItem.h"
#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"

// Sets default values
AChestWithItem::AChestWithItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChestSkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest Skeletal Mesh Comp"));
	RootComponent = ChestSkeletalMeshComp;
	ChestSkeletalMeshComp->SetSimulatePhysics(true);
	ChestSkeletalMeshComp->SetLinearDamping(6.f);

	ChestBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Chest Box Component"));
	ChestBoxComp->SetupAttachment(ChestSkeletalMeshComp);
	ChestBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ChestBoxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AChestWithItem::BeginPlay()
{
	Super::BeginPlay();

	OriginalScale = GetActorScale3D().X;
	SetActorScale3D(FVector(0.01f));

	ChestBoxComp->OnComponentBeginOverlap.AddDynamic(this, &AChestWithItem::OnComponentBeginOverlap);

	LockChestMaterial = UMaterialInstanceDynamic::Create(ChestSkeletalMeshComp->GetMaterial(3), this);
	ChestSkeletalMeshComp->SetMaterial(3, LockChestMaterial);
	ChestSkeletalMeshComp->SetMaterial(2, LockChestMaterial);
	if (ChestType == ECT_WithoutKey && OpenLockTexture) LockChestMaterial->SetTextureParameterValue(FName(TEXT("Texture")), OpenLockTexture);
}

// Called every frame
void AChestWithItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldMeshPopUp == true) PopUpMesh(DeltaTime);
}

#pragma region /////////////////// OPEN CHEST WHEN PLAYER IS COLLIDING WITH BOX //////////////////
void AChestWithItem::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanOpenChest(OtherActor) == false) return;

	if (OpenLockTexture) LockChestMaterial->SetTextureParameterValue(FName(TEXT("Texture")), OpenLockTexture);
	if (ChestOpenAnim) ChestSkeletalMeshComp->PlayAnimation(ChestOpenAnim, false);

	SpawnRandomItem();

	bIsOpen = true;
}

bool AChestWithItem::CanOpenChest(AActor* OtherActor)
{
	if (bIsOpen == true) return false;

	ATrianglePawn* TrianglePawn = Cast<ATrianglePawn>(OtherActor);
	if (TrianglePawn == nullptr) return false;

	if (ChestType == ECT_KeyRequired)
	{
		if (TrianglePawn->GetKeysAmount() <= 0) return false;
		TrianglePawn->AddKeys(-1);
	}

	return true;
}

void AChestWithItem::SpawnRandomItem()
{
	FVector SpawnLocation = GetActorLocation() + GetActorUpVector() * 100.f;
	int32 RandItem = FMath::RandRange(0, ListOfItems.Num() - 1);
	GetWorld()->SpawnActor<APickableItem>(ListOfItems[RandItem], SpawnLocation, FRotator(0.f));
}
#pragma endregion

void AChestWithItem::PopUpMesh(float Delta)
{
	if (PopUpTimeElapsed <= PopUpTime)
	{
		float NewScale = FMath::Lerp(0.01f, OriginalScale, PopUpTimeElapsed / PopUpTime);
		SetActorScale3D(FVector(NewScale));
		PopUpTimeElapsed += Delta;
	}
	else
	{
		SetActorScale3D(FVector(OriginalScale));
		bShouldMeshPopUp = false;
	}
}

