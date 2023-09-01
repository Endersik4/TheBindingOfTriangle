// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AwardDescriptionWidget.generated.h"

/**
 * 
 */
UCLASS()
class THEBINDINGOFTRIANGLE_API UAwardDescriptionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundHudImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemDescription;

	void SetItemDescriptionNewText(FText NewText);

protected:
	virtual void NativeConstruct() override; //konstruktor
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override; //Tick
};
