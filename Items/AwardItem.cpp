// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Items/AwardItem.h"

#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "TheBindingOfTriangle/BulletClasses/BulletComponent.h"

AAwardItem::AAwardItem()
{
	ItemType = ENone;
}

void AAwardItem::BeginPlay()
{
	Super::BeginPlay();
}

void AAwardItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateAwardMesh(DeltaTime);
}

void AAwardItem::RotateAwardMesh(float Delta)
{
	FQuat NewQuat = ItemMesh->GetRelativeRotation().Quaternion();
	FQuat PitchQuat(FVector(0.f, 0.f, 1.f), RotateSpeed * Delta);
	NewQuat *= PitchQuat;
	ItemMesh->SetRelativeRotation(NewQuat);
}

void AAwardItem::TakeItem(ATrianglePawn* TrianglePawn)
{
	TrianglePawn->SpawnItemDescriptionWidget(AwardDescription);

	if (bAddNewSlotForHeart == true)
	{
		TrianglePawn->AddSlotForHeart(SlotAmount, HeartNameToAddSlot);
		bAddNewSlotForHeart = false;
		Destroy();
		return;
	}

	for (EAwardAction Action : AwardAction)
	{
		if (Action == EAA_Multiply)
		{
			TrianglePawn->BulletComponent->GetBulletData() = TrianglePawn->BulletComponent->GetBulletData() * BulletData;
		}
		else if (Action == EAA_Divide)
		{
			TrianglePawn->BulletComponent->GetBulletData() = TrianglePawn->BulletComponent->GetBulletData() / BulletData;
		}
		else if (Action == EAA_Replace)
		{
			TrianglePawn->BulletComponent->GetBulletData() = BulletData;
		}
		else if (Action == EAA_ReplaceBulletClass)
		{
			TrianglePawn->BulletComponent->GetBulletData().BulletClass = BulletData.BulletClass;
			TrianglePawn->BulletComponent->GetBulletData().BulletIcon = BulletData.BulletIcon;
			TrianglePawn->RestartHudWidgetVariables();
			
		}
		else if (Action == EAA_ReplaceBulletType)
		{
			TrianglePawn->BulletComponent->GetBulletData().TypeOfBullet = BulletData.TypeOfBullet;

			TrianglePawn->BulletComponent->GetBulletData().ExplodeRadius = BulletData.ExplodeRadius;
			TrianglePawn->BulletComponent->GetBulletData().ExplosionParticle = BulletData.ExplosionParticle;
			TrianglePawn->BulletComponent->GetBulletData().ExplosionParticleScale = BulletData.ExplosionParticleScale;

			TrianglePawn->BulletComponent->GetBulletData().LaserTime = BulletData.LaserTime;
			TrianglePawn->BulletComponent->GetBulletData().LaserNiagaraParticle = BulletData.LaserNiagaraParticle;
		}
		else if (Action == EAA_ReplaceWayCurve)
		{
			TrianglePawn->BulletComponent->GetBulletData().bUseWayCurve = BulletData.bUseWayCurve;
			TrianglePawn->BulletComponent->GetBulletData().WayCurve = BulletData.WayCurve;
		}
		else if (Action == EAA_ReplaceShouldBack)
		{
			TrianglePawn->BulletComponent->GetBulletData().bShouldBack = BulletData.bShouldBack;
		}
		else if (Action == EAA_ReplaceHoldBullet)
		{
			TrianglePawn->BulletComponent->GetBulletData().bHoldBullet = BulletData.bHoldBullet;
			TrianglePawn->BulletComponent->GetBulletData().bShootHoldBulletAfterTime = BulletData.bShootHoldBulletAfterTime;
			TrianglePawn->BulletComponent->GetBulletData().HoldBulletTime = BulletData.HoldBulletTime;
			TrianglePawn->BulletComponent->GetBulletData().HoldBulletTriangleColor = BulletData.HoldBulletTriangleColor;
		}
		else if (Action == EAA_ReplaceAmountITD || (Action == EAA_ReplaceCircleAngleRadius && TrianglePawn->BulletComponent->GetBulletData().Amount == ExpectedBulletsAmount))
		{
			if (Action == EAA_ReplaceAmountITD) TrianglePawn->BulletComponent->GetBulletData().Amount = BulletData.Amount;
			TrianglePawn->BulletComponent->GetBulletData().DegreeBetween = BulletData.DegreeBetween;
			TrianglePawn->BulletComponent->GetBulletData().CirceAngle = BulletData.CirceAngle;
		}
	}

	Destroy();
}