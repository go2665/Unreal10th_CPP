// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopItemSellWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UShopItemSellWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// 드래그 완료
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
