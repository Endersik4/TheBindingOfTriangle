// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Widgets/HUDWidget.h"
#include "Components/Image.h"
#include "Components/TileView.h"

#include "TheBindingOfTriangle/Widgets/HeartTileViewObject.h"

void UHUDWidget::NativeConstruct()
{
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

			HeartsTileView->AddItem(NewHeartObject);
			NewHeartObject->ConditionalBeginDestroy();
		}
	}
}


