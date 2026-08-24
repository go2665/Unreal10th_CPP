// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Shop/ShopItemSellWidget.h"
#include "Widget/Inventory/InventoryDragDropOperation.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryComponent.h"

bool UShopItemSellWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	FInventoryCommandResult Result;
	if (IInventoryUserInterface* InventoryUI = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
	{
		if (UInventoryComponent* InvenComp = InventoryUI->GetInventoryComponent())
		{
			int32 TempIndex = InvenComp->GetTempSlotIndex();
			InventoryUI->ExecuteInventoryCommand(FInventoryCommand::MakeSell(TempIndex), Result);
		}
	}

	return Result.bSuccess;
}
