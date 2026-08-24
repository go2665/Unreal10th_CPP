// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHudWidget.generated.h"

class UPlayerStatBarsWidget;
class UInventoryWidget;
class UShopWidget;
class UDataTable;
/**
 * 
 */
UCLASS()
class UNREAL10TH_CPP_API UMainHudWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "UI|Inventory")
	void TestInventoryRefresh() const;
	
	UFUNCTION(BlueprintCallable, Category = "UI|Inventory")
	void ToggleInventory() const;

	UFUNCTION(BlueprintCallable, Category = "UI|Shop")
	void OpenShop();

	UFUNCTION(BlueprintCallable, Category = "UI|Shop")
	void CloseShop();

	UFUNCTION(BlueprintCallable, Category = "UI|Shop")
	void ToggleShop();

	UFUNCTION(BlueprintPure, Category = "UI|Shop")
	bool IsShopOpen() const;

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UPlayerStatBarsWidget> PlayerStatBars;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UInventoryWidget> Inventory;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UShopWidget> Shop;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|Shop")
	TObjectPtr<UDataTable> ShopItemList;
};
