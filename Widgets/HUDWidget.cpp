// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Widgets/HUDWidget.h"
#include "Components/Image.h"
#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "TheBindingOfTriangle/Widgets/HeartTileViewObject.h"
#include "TheBindingOfTriangle//TrianglePawnClasses/TrianglePawn.h"

void UHUDWidget::NativeConstruct()
{
	TrianglePawn = Cast<ATrianglePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}

void UHUDWidget::AddHeartToTile()
{
	HeartsTileView->ClearListItems();

	for (FHeartStruct NewHeart : CurrentHearts)
	{
		int32 CopyAmount = NewHeart.Amount;
		int32 Amount = (NewHeart.bEmptyTextureAfterDelete) ? FMath::RoundToInt(float(NewHeart.MaxAmount) / 2.f) : NewHeart.Amount;
		for (int i = 0; i != Amount; i++)
		{
			UHeartTileViewObject* NewHeartObject = NewObject<UHeartTileViewObject>(HeartTileViewObjectClass);
			if (CopyAmount >= 2)
			{
				NewHeartObject->HeartImageTex = NewHeart.WholeHeartTexture;
				CopyAmount -= 2;
			}
			else if (CopyAmount == 1)
			{
				NewHeartObject->HeartImageTex = NewHeart.HalfHeartTexture;
				CopyAmount = 0;
			}
			else if (NewHeart.bEmptyTextureAfterDelete == true && NewHeart.Amount <= NewHeart.MaxAmount)
			{
				NewHeartObject->HeartImageTex = NewHeart.EmptyHeartTexture;
			}
			else continue;

			HeartsTileView->AddItem(NewHeartObject);
			NewHeartObject->ConditionalBeginDestroy();
		}
	}
}

void UHUDWidget::SetBulletIconImage(UTexture2D* NewBulletIcon)
{
	BulletIconImage->SetBrushFromTexture(NewBulletIcon);
}

void UHUDWidget::GetItemsAmount()
{
	if (TrianglePawn == nullptr) return;

	CoinsAmount->SetText(AddZeroToText(TrianglePawn->GetCoinsAmount()));
	BombsAmount->SetText(AddZeroToText(TrianglePawn->GetBombsAmount()));
	KeysAmount->SetText(AddZeroToText(TrianglePawn->GetKeysAmount()));
}

FText UHUDWidget::AddZeroToText(int32 Amount)
{
	FString Text;
	if (Amount < 10) Text = "0";

	Text += FString::FromInt(Amount);

	return FText::FromString(Text);
}


