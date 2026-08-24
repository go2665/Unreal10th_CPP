// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Data/Item/ItemDataAsset.h"
#include "ShopTable.generated.h"

USTRUCT(BlueprintType)
struct FShopTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 아이템 종류
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	TObjectPtr<const UItemDataAsset> ItemData = nullptr;

	// 아이템이 상점에 등장할 가중치(1이 기본값. 숫자가 크면 클수록 잘나온다.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop", meta = (ClampMin = "0"))
	float SelectWeight = 1.0f;

	// 구매 가능한 아이템 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop", meta = (ClampMin = "1"))
	int32 StockCount = 1;
};
