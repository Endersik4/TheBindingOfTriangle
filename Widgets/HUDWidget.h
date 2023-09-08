// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TheBindingOfTriangle/TrianglePawnClasses/TrianglePawn.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBINDINGOFTRIANGLE_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UHeartTileViewObject> HeartTileViewObjectClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundHudImage;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTileView* HeartsTileView;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CoinsAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* BombsAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeysAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* BulletIconImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* MinimapImage;
	
	TArray<FHeartStruct> CurrentHearts;

	void SetBulletIconImage(UTexture2D* NewBulletIcon);

	void CallAddHeartToTile() { AddHeartToTile(); }

	void GetItemsAmount();

protected:
	virtual void NativeConstruct() override; //konstruktor
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override; //Tick

private:
	void AddHeartToTile();

	FText AddZeroToText(int32 Amount);

	class ATrianglePawn* TrianglePawn;
};
