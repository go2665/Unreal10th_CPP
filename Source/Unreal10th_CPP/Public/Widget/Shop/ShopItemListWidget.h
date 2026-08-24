// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopItemListWidget.generated.h"

class UVerticalBox;
class UShopItemBuyWidget;
/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UShopItemListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void ResetItemList(const UDataTable* InItemList);
	void UpdateAllBuyButton();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> ItemBuyContainer;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<UShopItemBuyWidget>> ShopItems;
	
};
