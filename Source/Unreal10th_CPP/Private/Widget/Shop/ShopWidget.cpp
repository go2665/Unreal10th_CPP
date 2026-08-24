// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Shop/ShopWidget.h"
#include "Widget/Shop/ShopItemSellWidget.h"
#include "Widget/Shop/ShopItemListWidget.h"
#include "Components/Button.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Exit)
	{
		Exit->OnClicked.AddDynamic(this, &UShopWidget::OnExitButtonClicked);
	}
}

void UShopWidget::InitializeShop(UDataTable* ItemList)
{
	ShopItemList = ItemList;
	ResetShopItemListWidget();
}

void UShopWidget::UpdateAllBuyButtonState(int32 _)
{
	//UE_LOG(LogTemp, Log, TEXT("UpdateAllBuyButtonState"));
	if (GetVisibility() == ESlateVisibility::Visible
		|| GetVisibility() == ESlateVisibility::SelfHitTestInvisible)	// 자신이 열려있으면
	{
		//UE_LOG(LogTemp, Log, TEXT("UpdateAllBuyButtonState : Visible"));
		ItemListWidget->UpdateAllBuyButton();	// 아이템 리스트 위젯에게 구매 버튼 업데이트 하라고 지시
	}
}

void UShopWidget::ResetShopItemListWidget()
{
	if (ShopItemList)
	{
		ItemListWidget->ResetItemList(ShopItemList);
	}
}

void UShopWidget::OnExitButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("OnExitButtonClicked"));
	SetVisibility(ESlateVisibility::Collapsed);
	OnShopClosed.Broadcast();
}
