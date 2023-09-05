// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Widgets/RoomTileViewEntry.h"
#include "Components/Image.h"

#include "TheBindingOfTriangle/Widgets/MinimapRoomObject.h"

void URoomTileViewEntry::NativeConstruct()
{
}

void URoomTileViewEntry::NativeOnInitialized()
{
}

void URoomTileViewEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UMinimapRoomObject* RoomObject = Cast<UMinimapRoomObject>(ListItemObject);
	if (RoomObject == nullptr) return;

	RoomImage->SetBrushFromTexture(RoomObject->RoomImageTex);
}
