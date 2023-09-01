// Fill out your copyright notice in the Description page of Project Settings.


#include "TheBindingOfTriangle/Widgets/AwardDescriptionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAwardDescriptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAwardDescriptionWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
}

void UAwardDescriptionWidget::SetItemDescriptionNewText(FText NewText)
{
	ItemDescription->SetText(NewText);
}
