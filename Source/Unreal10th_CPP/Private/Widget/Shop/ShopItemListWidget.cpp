// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Shop/ShopItemListWidget.h"
#include "Widget/Shop/ShopItemBuyWidget.h"
#include "CommonHeader/ShopTable.h"
#include "Components/VerticalBox.h"

void UShopItemListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemBuyContainer)
	{
		int Count = ItemBuyContainer->GetChildrenCount();
		ShopItems.Empty(Count);
		for (int32 i = 0; i < Count; i++)
		{
			UShopItemBuyWidget* itemWidget = Cast<UShopItemBuyWidget>(ItemBuyContainer->GetChildAt(i));
			ShopItems.Add(itemWidget);
		}
	}
}

void UShopItemListWidget::ResetItemList(const UDataTable* InItemList)
{
	// 전체 가중치 계산
	float TotalWeight = 0.0f;
	TArray<FShopTableRow*> AllRows;
	InItemList->GetAllRows(TEXT("ShopDataTable"), AllRows);
	for (const FShopTableRow* Row : AllRows)
	{
		TotalWeight += Row->SelectWeight;
	}

	// 가중치에 따라 ShopItems갯수만큼 데이터 선택
	int32 SelectCount = ShopItems.Num();
	TArray<const FShopTableRow*> SelectedRows;
	SelectedRows.Empty(SelectCount);
	for (int32 i = 0; i < SelectCount; i++)
	{
		float Hit = FMath::FRandRange(0.0f, TotalWeight);
		float CurrentWeight = 0.0f;
		for (const FShopTableRow* Row : AllRows)
		{
			CurrentWeight += Row->SelectWeight;
			if (Hit < CurrentWeight)
			{
				SelectedRows.Add(Row);
				break;
			}
		}
	}

	// 선택된 데이터에 따라 UI 갱신
	for (int32 i = 0; i < SelectCount; i++)
	{
		const FShopTableRow* row = SelectedRows[i];
		ShopItems[i]->InitializeItemBuy(row->ItemData, row->StockCount);
	}	
}

void UShopItemListWidget::UpdateAllBuyButton()
{
	for (const UShopItemBuyWidget* item : ShopItems)	// 자신이 가진 판매 아이템 목록들의 구매버턴 업데이트 지시
	{
		item->UpdateBuyButton();
	}
}
