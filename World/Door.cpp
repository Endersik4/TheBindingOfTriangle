// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/Door.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DoorMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh Component"));
	RootComponent = DoorMeshComp;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::ChangeDoor()
{
	if (!DoorOpenMesh || !DoorClosedMesh || !DoorKeyMesh) return;

	switch (DoorStatus)
	{
	case EDS_Open: DoorMeshComp->SetStaticMesh(DoorOpenMesh); break;
	case EDS_Closed: DoorMeshComp->SetStaticMesh(DoorClosedMesh); break;
	case EDS_KeyRequired: DoorMeshComp->SetStaticMesh(DoorKeyMesh); break;
	default: UE_LOG(LogTemp, Warning, TEXT("DOOR STATUS WRONG"));
	}

	if (!DoorRoomMaterial || !DoorAwardMaterial || !DoorBossMaterial) return;

	switch (DoorType)
	{
	case EDT_Room: DoorMeshComp->SetMaterial(0, DoorRoomMaterial); break;
	case EDT_Boss: DoorMeshComp->SetMaterial(0, DoorBossMaterial); break;
	case EDT_Award: DoorMeshComp->SetMaterial(0, DoorAwardMaterial); break;
	default: UE_LOG(LogTemp, Warning, TEXT("DOOR TYPE WRONG"));
	}

}

