// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MinimapRoomObject.generated.h"

/**
 * 
 */
UCLASS()
class THEBINDINGOFTRIANGLE_API UMinimapRoomObject : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* RoomImageTex;
};
