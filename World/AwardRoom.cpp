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
}

void AAwardRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAwardRoom::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBoxBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (bAwardSpawned == false) PickRandomAwardFromList();
}

void AAwardRoom::PickRandomAwardFromList()
{
	int32 RandChoice = FMath::RandRange(0, RandomAwardItemList.Num() - 1);
	GetWorld()->SpawnActor<AAwardItem>(RandomAwardItemList[RandChoice], PedestalForItemMesh->GetComponentLocation(), FRotator(0.f));
	bAwardSpawned = true;
}
