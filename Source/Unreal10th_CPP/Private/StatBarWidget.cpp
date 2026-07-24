#include "StatBarWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

// Fill out your copyright notice in the Description page of Project Settings.

void UStatBarWidget::TestImageColorChange(FLinearColor InColor)
{
	TestImage->SetColorAndOpacity(InColor);
}

void UStatBarWidget::TestTextChange(FText InString)
{
	TestTextBlock->SetText(InString);
}
