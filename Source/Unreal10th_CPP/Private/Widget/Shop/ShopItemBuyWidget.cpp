// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Shop/ShopItemBuyWidget.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryComponent.h"
#include "Data/Item/ItemDataAsset.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Components/Overlay.h"
#include "Components/Button.h"

void UShopItemBuyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemCount)
	{
		ItemCount->SetHintText(FText::AsNumber(MinimumBuyCount));
		ItemCount->OnTextChanged.AddDynamic(this, &UShopItemBuyWidget::OnItemCountTextChanged);		// 변경이 있을 때
		ItemCount->OnTextCommitted.AddDynamic(this, &UShopItemBuyWidget::OnItemCountTextCommitted);	// 변경을 확정했을 때(엔터친 후, 포커스를 잃은 후)
	}

	if (ItemBuy)
	{
		ItemBuy->OnClicked.AddDynamic(this, &UShopItemBuyWidget::OnBuyButtonClicked);
	}

	if (SoldOut)
	{
		SoldOut->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UShopItemBuyWidget::InitializeItemBuy(const UItemDataAsset* InItemData, int32 InStockCount)
{
	// 아직 데이터(Icon 등)가 로딩되지 않은 경우 비동기 로딩 요청
	if (InItemData && !InItemData->IsLoaded())
	{
		InItemData->RequestDataLoad(
			FStreamableDelegate::CreateWeakLambda(
				this,
				[this]()
				{
					// 비동기 로딩 완료 후 다시 한 번 UI 리프레시
					RefreshItemBuyWidget();
				})
		);
	}	

	ItemData = InItemData;
	RefreshItemBuyWidget();

	SetStockCount(InStockCount);
	SetBuyCount(MinimumBuyCount);

	UpdateBuyButton();
}

void UShopItemBuyWidget::OnItemCountTextChanged(const FText& InText)
{
	UE_LOG(LogTemp, Log, TEXT("Changed : %s"), *InText.ToString());

	FString number = InText.ToString();
	if (number.IsNumeric())
	{		
		SetBuyCount(FMath::Clamp(FCString::Atoi(*number), MinimumBuyCount, StockCount));	// 갯수는 1~StockCount 사이
		UpdateBuyButton();
	}
}

void UShopItemBuyWidget::OnBuyButtonClicked()
{
	//UE_LOG(LogTemp, Log, TEXT("구매 버튼 클릭"));

	if (IInventoryUserInterface* InventoryUI = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
	{
		if (UInventoryComponent* InvenComp = InventoryUI->GetInventoryComponent())
		{
			int32 TempIndex = InvenComp->GetTempSlotIndex();
			FInventoryCommandResult ResultAdd;
			InventoryUI->ExecuteInventoryCommand(FInventoryCommand::MakeAdd(ItemData.Get(), BuyCount), ResultAdd);			// 구매한 아이템을 인벤토리에 추가
			FInventoryCommandResult ResultMoney;
			InventoryUI->ExecuteInventoryCommand(FInventoryCommand::MakeMoney(-ItemData->Price * BuyCount), ResultMoney);	// 구매한 아이템의 가격만큼 돈을 차감

			SetStockCount(StockCount - BuyCount);
			SetBuyCount(MinimumBuyCount);
			if (StockCount < MinimumBuyCount)
			{
				// 갯수 입력 금지
				ItemCount->SetIsEnabled(false);

				// 매진 표시
				SoldOut->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
	}
}

void UShopItemBuyWidget::SetStockCount(int32 InCount)
{
	StockCount = InCount;
	ItemStockCount->SetText(FText::AsNumber(StockCount));
	UpdateBuyButton();
}

void UShopItemBuyWidget::SetBuyCount(int32 InCount)
{
	BuyCount = InCount;
	if (BuyCount > MinimumBuyCount)
	{
		ItemCount->SetText(FText::AsNumber(BuyCount));
	}
	else
	{
		ItemCount->SetText(FText::GetEmpty());
	}
}

void UShopItemBuyWidget::RefreshItemBuyWidget() const
{
	ItemCount->SetIsEnabled(true);
	SoldOut->SetVisibility(ESlateVisibility::Hidden);

	if (ItemData.IsValid())
	{
		ItemIcon->SetBrushFromTexture(ItemData->Icon.Get());
		ItemName->SetText(ItemData->DisplayName);
		ItemPrice->SetText(FText::AsNumber(ItemData->Price));
		ItemDescription->SetText(ItemData->Description);
	}
	else
	{
		ItemIcon->SetBrushFromTexture(nullptr);
		ItemName->SetText(FText::GetEmpty());
		ItemPrice->SetText(FText::GetEmpty());
		ItemDescription->SetText(FText::GetEmpty());
	}
}

void UShopItemBuyWidget::UpdateBuyButton() const
{
	if (StockCount < MinimumBuyCount)
	{
		ItemBuy->SetIsEnabled(false);
	}
	else
	{
		if (IInventoryUserInterface* InventoryUI = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
		{
			if (UInventoryComponent* InvenComp = InventoryUI->GetInventoryComponent())
			{
				bool hasEnoughMoney = InvenComp->GetMoney() >= (BuyCount * ItemData->Price);
				ItemBuy->SetIsEnabled(hasEnoughMoney);
			}
		}
	}
}
