// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/AwardRoom.h"
#include "Components/StaticMeshComponent.h"

#include "TheBindingOfTriangle/Items/AwardItem.h"

AAwardRoom::AAwardRoom()
{
	PedestalForItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pedestal For Item Mesh"));
	PedestalForItemMesh->SetupAttachment(RootComponent);
}

void AAwardRoom::BeginPlay()
{
	Super::BeginPlay();
	PickRandomAwardFromList();
}

void AAwardRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAwardRoom::PickRandomAwardFromList()
{
	int32 RandChoice = FMath::RandRange(0, RandomAwardItemList.Num() - 1);
	GetWorld()->SpawnActor<AAwardItem>(RandomAwardItemList[RandChoice], PedestalForItemMesh->GetComponentLocation(), FRotator(0.f));
}
