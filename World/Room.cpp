// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/Room.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "TheBindingOfTriangle/World/Door.h"
#include "TheBindingOfTriangle/EnemyClasses/BaseEnemy.h"
#include "TheBindingOfTriangle/TrianglePawnClasses/MinimapActor.h"

// Sets default values
ARoom::ARoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor Mesh Component"));
	RootComponent = FloorMeshComp;

	TopWallMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Top Wall Mesh Component"));
	TopWallMeshComp->SetupAttachment(RootComponent);

	BottomWallMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bottom Wall Mesh Component"));
	BottomWallMeshComp->SetupAttachment(RootComponent);

	RightWallMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Wall Mesh Component"));
	RightWallMeshComp->SetupAttachment(RootComponent);

	LeftWallMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Wall Mesh Component"));
	LeftWallMeshComp->SetupAttachment(RootComponent);

	ActivateRoomBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Activate Room Box Component"));
	ActivateRoomBoxComp->SetupAttachment(RootComponent);
	ActivateRoomBoxComp->SetCollisionProfileName(FName(TEXT("Custom...")));
	ActivateRoomBoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ActivateRoomBoxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);

	CameraLocationBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Camera Location Box Component"));
	CameraLocationBoxComp->SetupAttachment(RootComponent);
	CameraLocationBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CameraLocationBoxComp->SetComponentTickEnabled(false);
}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();

	TrianglePawn = Cast<ATrianglePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (TrianglePawn == nullptr) UE_LOG(LogTemp, Error, TEXT("TRIANGLE PAWN CASTED FAILED IN ROOM %s"), *GetFName().ToString());

	ActivateRoomBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ARoom::OnBoxBeginOverlap);
	ActivateRoomBoxComp->OnComponentEndOverlap.AddDynamic(this, &ARoom::OnBoxEndOverlap);

	SpawnDoors();

}
	
// Called every frame
void ARoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoom::SpawnDoors()
{
	for (int i = 0; i != 4; i++)
	{
		if (RoomData.DoorsType[i] == EDT_None) continue;
		FTransform NewTransfrom = GetProperTransform(i);

		ADoor* SpawnedDoor = GetWorld()->SpawnActorDeferred<ADoor>(DoorClass, NewTransfrom);
		SpawnedDoor->SetDoorType(RoomData.DoorsType[i]);
		SpawnedDoor->FinishSpawning(NewTransfrom);
		DoorData.Add(SpawnedDoor);
	}
}

FTransform ARoom::GetProperTransform(int32 Index)
{
	if (Index == 0)
	{
		if (TopDoorWallStaticMesh) TopWallMeshComp->SetStaticMesh(TopDoorWallStaticMesh);
		return FTransform(FRotator(0, 90.f, 0), GetActorLocation() + GetActorForwardVector() * 540);
	}
	else if (Index == 1)
	{
		if (TopDoorWallStaticMesh) BottomWallMeshComp->SetStaticMesh(TopDoorWallStaticMesh);
		return FTransform(FRotator(0, -90.f, 0), GetActorLocation() + -GetActorForwardVector() * 540);
	}
	else if (Index == 2)
	{
		if (RightDoorWallStaticMesh) RightWallMeshComp->SetStaticMesh(RightDoorWallStaticMesh);
		return FTransform(FRotator(0, 180.f, 0), GetActorLocation() + GetActorRightVector() * 1080);
	}
	else
	{
		if (RightDoorWallStaticMesh) LeftWallMeshComp->SetStaticMesh(RightDoorWallStaticMesh);
		return FTransform(FRotator(0), GetActorLocation() + -GetActorRightVector() * 1080);
	}
}

#pragma region ///////////////// ACTIVATE ROOM BOX ///////////////////
void  ARoom::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TrianglePawn == nullptr) return;
	TrianglePawn->ChangeCameraRoom(true, CameraLocationBoxComp->GetComponentLocation());

	if (MinimapActor) MinimapActor->ChangeCurrentRoom(RoomData.Location);

	// Change Player location so he cant stuck in between doors
	FVector DirForImpulse = UKismetMathLibrary::FindLookAtRotation(TrianglePawn->GetActorLocation(), GetActorLocation()).Vector();
	DirForImpulse.Z = 0.f;
	TrianglePawn->SetActorLocation(DirForImpulse * 120.f + TrianglePawn->GetActorLocation());

	if (ListOfEnemies.Num() == 0 || RoomData.RoomType == ERT_Spawn) return;

	bool bEmptyRoom = FMath::FRandRange(0.f, 100.f) < 20.f ? true : false;
	if (bEmptyRoom == true)
	{
		bShouldSpawnEnemies = false;
		return;
	}

	GetWorldTimerManager().SetTimer(TimeToSpawnEnemiesHandle, this, &ARoom::SpawnEnemiesAfterTime, SpawnEnemiesTime, false);
}

void  ARoom::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("OFF"));
}

#pragma endregion


void ARoom::CloseDoors(bool bClose)
{
	for (ADoor* CurrentDoor : DoorData)
	{
		if (bClose) CurrentDoor->SetDoorStatus(EDS_Closed);
		else CurrentDoor->SetDoorStatus(EDS_Open);
	}
}

void ARoom::SpawnEnemiesAfterTime()
{
	if (bShouldSpawnEnemies == false) return;

	CloseDoors(true);
	SpawnEnemies();
}

void ARoom::SpawnEnemies()
{
	int32 NumOfEnemies = FMath::RandRange(RangeOfEnemiesToSpawn.GetLowerBoundValue(), RangeOfEnemiesToSpawn.GetUpperBoundValue());
	
	FVector RandLocation;
	RandLocation.Z = 98.f;
	for (int i = 0; i != NumOfEnemies; i++)
	{
		int32 EnemyChoice = FMath::RandRange(0, ListOfEnemies.Num() - 1);
		RandLocation.X = FMath::RandRange(-540.f, 540.f);
		RandLocation.Y = FMath::RandRange(-1100.f, 1100.f);
		RandLocation = UKismetMathLibrary::TransformLocation(GetActorTransform(), RandLocation);
		ABaseEnemy* SpawnedEnemy = GetWorld()->SpawnActor<ABaseEnemy>(ListOfEnemies[EnemyChoice], RandLocation, FRotator(0.f, 45.f, 0.f));
		if (SpawnedEnemy)
		{
			SpawnedEnemy->SetCurrentRoom(this);
			if (SpawnedEnemy->GetEnemyActionWhenSpawned() != ESA_BouncesOfWalls) SpawnedEnemy->SetActorRotation(FRotator(0.f));
			HowManyEnemiesLeft++;
		}
	}
}

void ARoom::AddHowManyEnemisSet(int32 Add)
{
	HowManyEnemiesLeft += Add;
	if (HowManyEnemiesLeft <= 0)
	{
		bShouldSpawnEnemies = false;
		CloseDoors(false);
	}
}