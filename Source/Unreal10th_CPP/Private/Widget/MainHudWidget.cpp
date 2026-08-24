// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MainHudWidget.h"
#include "Widget/Inventory/InventoryWidget.h"
#include "Widget/Shop/ShopWidget.h"
#include "Component/InventoryComponent.h"
#include "Interface/InventoryUserInterface.h"

void UMainHudWidget::TestInventoryRefresh() const
{
#if WITH_EDITOR
	if (Inventory)
	{
		Inventory->TestRefresh();
	}
#endif
}

void UMainHudWidget::ToggleInventory() const
{
	if (Inventory)
	{
		Inventory->ToggleInventoryWidget();
	}
}

void UMainHudWidget::OpenShop()
{
	if (Shop)
	{
		Shop->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainHudWidget::CloseShop()
{
	if (Shop)
	{
		Shop->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainHudWidget::ToggleShop()
{
	if (IsShopOpen())
	{
		CloseShop();
	}
	else
	{
		OpenShop();
	}
}

bool UMainHudWidget::IsShopOpen() const
{
	return Shop && (Shop->GetVisibility() == ESlateVisibility::Visible);
}

void UMainHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
		
	if (Shop)
	{
		Shop->InitializeShop(ShopItemList);
		FInventoryCommandResult Result;
		if (IInventoryUserInterface* InventoryUI = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
		{
			if (UInventoryComponent* InvenComp = InventoryUI->GetInventoryComponent())
			{
				InvenComp->OnMoneyChanged.AddUObject(Shop, &UShopWidget::UpdateAllBuyButtonState);
			}
		}
	}
}
