// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventorySlotWidget.h"
#include "Widget/Inventory/InventoryDragDropOperation.h"
#include "Widget/Inventory/TemporarySlotWidget.h"
#include "Component/InventoryComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UInventorySlotWidget::InitializeSlot(UInventoryComponent* InInven, int32 InIndex)
{
	if (!InInven) return;

	TargetInventory = InInven;
	Index = InIndex;

	RefreshSlot();
}

void UInventorySlotWidget::RefreshSlot() const
{
	if (!TargetInventory.IsValid()) return;

	const FInvenSlot* TargetSlot = TargetInventory->GetSlot(Index);
	if (!TargetSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Slot %d]가 null입니다."), Index);
		return;
	}

	if (TargetSlot->IsEmpty())
	{
		// 슬롯이 비어있으면
		IconImage->SetBrushFromTexture(nullptr);
		IconImage->SetBrushTintColor(FLinearColor::Transparent);
		CountBox->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		// 슬롯이 비어있지 않으면
		IconImage->SetBrushFromTexture(TargetSlot->ItemData->Icon.Get());
		IconImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));

		CountText->SetText(FText::AsNumber(TargetSlot->GetCount()));
		MaxStackText->SetText(FText::AsNumber(TargetSlot->ItemData->MaxStackCount));
		CountBox->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	//UE_LOG(LogTemp, Log, TEXT("OnMouseEnter : %d 슬롯"), Index);
	OnSlotEnter.Broadcast(Index);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnSlotLeave.Broadcast();
	//UE_LOG(LogTemp, Log, TEXT("OnMouseLeave : %d 슬롯"), Index);
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	FInvenSlot* InvenSlot = TargetInventory->GetSlot(Index);
	if (!InvenSlot || !InvenSlot->ItemData) return;

	UE_LOG(LogTemp, Log, TEXT("드래그가 %d 슬롯에서 시작"), Index);

	UInventoryDragDropOperation* DragOp = NewObject<UInventoryDragDropOperation>();
	DragOp->StartIndex = Index;

	UTemporarySlotWidget* DragTempWidget = CreateWidget<UTemporarySlotWidget>(
		this,
		TargetInventory->GetTemporasySlotWidgetClass()
	);
	DragTempWidget->SetVisual(InvenSlot->ItemData->Icon.Get(), InvenSlot->GetCount());
	DragOp->DefaultDragVisual = DragTempWidget;
	OutOperation = DragOp;	// NativeOnDrop과 NativeOnDragCancelled를 발동시키기 위해 필수

	FInventoryCommandResult Result;
	TargetInventory->ExecuteCommand(FInventoryCommand::MakeMove(Index, TargetInventory->GetTempSlotIndex()), Result);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Log, TEXT("드래그가 %d 슬롯에서 종료"), Index);
	//return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	FInventoryCommandResult Result;
	TargetInventory->ExecuteCommand(FInventoryCommand::MakeMove(TargetInventory->GetTempSlotIndex(), Index), Result);

	UInventoryDragDropOperation* Op = Cast< UInventoryDragDropOperation>(InOperation);
	TargetInventory->ExecuteCommand(FInventoryCommand::MakeMove(TargetInventory->GetTempSlotIndex(), Op->StartIndex ), Result);

	return true;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UE_LOG(LogTemp, Log, TEXT("바닥에서 드래그 종료"));
	
	if (APlayerController* PC = GetOwningPlayer())
	{
		UE_LOG(LogTemp, Log, TEXT("플레이어 컨트롤러 확인"));
		//FHitResult HitResult;
		//if (PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))	// UI에서 관리하는 마우스 좌표와 PC가 관리하는 마우스 좌표가 다름
		//{
		//	UE_LOG(LogTemp, Log, TEXT("바닥 히트 성공"));
		//	FInventoryCommandResult Result;
		//	TargetInventory->ExecuteCommand(
		//		FInventoryCommand::MakeDrop(TargetInventory->GetTempSlotIndex(), HitResult.Location), 
		//		Result);
		//}

		FVector2D AbsolutePosition = InDragDropEvent.GetScreenSpacePosition();
		FVector2D PixelPosion;
		FVector2D ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(this, AbsolutePosition, PixelPosion, ViewportPosition);

		FVector WorldLocation;
		FVector WorldDirection;
		if (PC->DeprojectScreenPositionToWorld(
			PixelPosion.X, PixelPosion.Y,
			WorldLocation, WorldDirection))
		{
			FVector Start = WorldLocation;
			FVector End = Start + WorldDirection * 10000.0f;

			FHitResult HitResult;
			FVector SpawnLocation;
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
			{
				SpawnLocation = HitResult.Location;
			}
			else
			{
				SpawnLocation = End;
			}

			// 일정 거리 이상 멀어지는 것 방지
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				const float MaxDistance = 500.0f;
				FVector PlayerLocation = PlayerPawn->GetActorLocation();
				if (FVector::DistSquared2D(SpawnLocation, PlayerLocation) > MaxDistance * MaxDistance)
				{
					FVector Direction = (SpawnLocation - PlayerLocation).GetSafeNormal2D();
					SpawnLocation = PlayerLocation + Direction * MaxDistance;

					// SpawnLocation이 벽 안이 되는 것을 방지
					FVector DownStart = SpawnLocation + FVector::UpVector * 10000.0f;
					FVector DownEnd = SpawnLocation + FVector::DownVector * 10000.0f;
					FHitResult GroundHit;
					if (GetWorld()->LineTraceSingleByChannel(GroundHit, DownStart, DownEnd, ECollisionChannel::ECC_Visibility))
					{
						SpawnLocation = GroundHit.Location;	
					}
				}
			}

			// 실제 드랍 생성
			FInventoryCommandResult Result;
			TargetInventory->ExecuteCommand(
				FInventoryCommand::MakeDrop(TargetInventory->GetTempSlotIndex(), SpawnLocation),
				Result);
		}
	}

	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 마우스 버튼이 눌려지면 실행되는 함수
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))	
	{
		// 마우스 왼쪽이 눌려져 있다.
		if (FInvenSlot* InvenSlot = TargetInventory->GetSlot(Index))
		{
			if (!InvenSlot->IsEmpty())
			{
				return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
			}
		}
	}
	else if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		// 마우스 오른쪽이 눌려져 있다.
		if (FInvenSlot* InvenSlot = TargetInventory->GetSlot(Index))
		{
			if (!InvenSlot->IsEmpty())
			{
				FInventoryCommandResult Result;
				TargetInventory->ExecuteCommand(FInventoryCommand::MakeUse(Index), Result);
				TargetInventory->ExecuteCommand(FInventoryCommand::MakeEquip(Index), Result);
			}
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
