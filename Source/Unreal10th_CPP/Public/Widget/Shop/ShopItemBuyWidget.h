// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopItemBuyWidget.generated.h"

class UImage;
class UTextBlock;
class UEditableTextBox;
class UButton;
class UOverlay;
class UItemDataAsset;
/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UShopItemBuyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 판매 아이템 위젯 초기화
	void InitializeItemBuy(const UItemDataAsset* InItemData, int32 InStockCount);

	// 버튼의 활성화/비활성화를 업데이트하는 함수
	void UpdateBuyButton() const;

private:
	UFUNCTION()
	void OnItemCountTextChanged(const FText& InText);

	UFUNCTION()
	void OnBuyButtonClicked();	

	// StockCount의 값과 ItemStockCount를 동시에 세팅하는 함수
	void SetStockCount(int32 InCount);

	// BuyCount의 값과 ItemCount를 동시에 세팅하는 함수
	void SetBuyCount(int32 InCount);

	// UI를 갱신하는 함수
	void RefreshItemBuyWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemPrice = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemStockCount = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescription = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ItemCount = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UButton> ItemBuy = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Shop|ItemSell", meta = (BindWidget))
	TObjectPtr<UOverlay> SoldOut = nullptr;

private:
	// 최소 구매 갯수
	static constexpr int32 MinimumBuyCount = 1;

	// 현재 구매 가능한 갯수
	int32 StockCount = 0;

	// 현재 사려고 하는 갯수
	int32 BuyCount = MinimumBuyCount;

	// 현재 사려고하는 아이템의 데이터에셋
	TWeakObjectPtr<const UItemDataAsset> ItemData = nullptr;

};
