// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/World/MakeRooms.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMakeRooms::AMakeRooms()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMakeRooms::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AMakeRooms::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMakeRooms::SpawnDebugGrid()
{
	DrawDebugBox(GetWorld(), GetActorLocation(), FVector(200.f, 200.f, 0.f), FColor::White, true);
	for (int i = 0; i != FriendsAmount; i++)
	{
		UE_LOG(LogTemp, Error, TEXT("NEW FRIEND"));

		FVector Location = GetActorLocation();
		for (int j = 0; j != HowManyRooms; j++)
		{
			int32 Side = UKismetMathLibrary::RandomIntegerInRange(1, 4); // 1 - Forward, 2 - Back, 3 - Right, 4 - Left 

			FVector Dir;

			switch (Side)
			{
			case 1: Dir = GetActorForwardVector(); break;
			case 2: Dir = -GetActorForwardVector(); break;
			case 3: Dir = GetActorRightVector(); break;
			case 4: Dir = -GetActorRightVector(); break;
			}

			if (GridData.Find(Location + Dir * 400.f) != nullptr)
			{
				j--;
				continue;
			}

			UE_LOG(LogTemp, Warning, TEXT("SIDE %d"), Side);
			Location += Dir * 400.f;

			FGrid NewGrid(Location, false);
			GridData.Add(Location, NewGrid);

			DrawDebugBox(GetWorld(), Location, FVector(200.f, 200.f, 0.f), FColor::White, true);
		}
	}

}

