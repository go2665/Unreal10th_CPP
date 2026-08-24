// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/Action/ItemAction_Heal.h"
#include "Component/StatComponent.h"
#include "Interface/StatInterface.h"

void UItemAction_Heal::ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
	UE_LOG(LogTemp, Log, TEXT("%s에게 %.1f만큼 체력 회복"), *InTarget->GetName(), HealAmount);

	if (IStatInterface* Stat = Cast<IStatInterface>(InTarget))
	{
		if (UStatComponent* StatComp = Stat->GetStatComponent())
		{
			IHealthInterface::Execute_HealHealth(StatComp, HealAmount);
		}
	}
}
