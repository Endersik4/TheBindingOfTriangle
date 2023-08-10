// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Widgets/HeartTileViewEntry.h"
#include "Components/Image.h"

#include "TheBindingOfTriangle/Widgets/HeartTileViewObject.h"

void UHeartTileViewEntry::NativeConstruct()
{

}

void UHeartTileViewEntry::NativeOnInitialized()
{
}

void UHeartTileViewEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UHeartTileViewObject* NewHeartObject = Cast<UHeartTileViewObject>(ListItemObject);
	if (NewHeartObject == nullptr) return;

	HeartImage->SetBrushFromTexture(NewHeartObject->HeartImageTex);
}
