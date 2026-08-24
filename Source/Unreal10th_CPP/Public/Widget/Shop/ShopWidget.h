// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopClosed);

class UShopItemSellWidget;
class UShopItemListWidget;
class UButton;
/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void InitializeShop(UDataTable* ItemList);
		
	UFUNCTION()
	void UpdateAllBuyButtonState(int32 _);

protected:
	void ResetShopItemListWidget();

private:
	UFUNCTION()
	void OnExitButtonClicked();

public:
	FOnShopClosed OnShopClosed;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Shop|Sell", meta = (BindWidget))
	TObjectPtr<UShopItemSellWidget> ItemSellWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|Buy", meta = (BindWidget))
	TObjectPtr<UShopItemListWidget> ItemListWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Shop", meta = (BindWidget))
	TObjectPtr<UButton> Exit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Buy")
	TObjectPtr<UDataTable> ShopItemList = nullptr;
};
