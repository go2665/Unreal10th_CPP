// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	//OnStaminaEmpty.AddDynamic(this, &UStatComponent::StaminaAutoRecovertyPerTick);
	//FScriptDelegate Test;
	//Test.BindUFunction(this, FName("UStatComponent::StaminaAutoRecovertyPerTick"));
	//OnStaminaEmpty.Add(Test);
}

void UStatComponent::InitializeStat(FAutoRecoveryData& InData)
{
	CurrentStamina = MaxStamina;
	StaminaRecoveryData = InData;
}

float UStatComponent::GetCurrentStamina_Implementation() const
{
	return CurrentStamina;
}

float UStatComponent::GetMaxStamina_Implementation() const
{
	return MaxStamina;
}

bool UStatComponent::ConsumeStamina_Implementation(float InAmount)
{
	bool bResult = false;
	if (CurrentStamina >= InAmount)
	{
		CurrentStamina -= InAmount;

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(
			StaminaAutoRecoveryTimerHandle,
			this,
			&UStatComponent::StaminaAutoRecovertyPerTick,
			StaminaRecoveryData.TickInterval,
			true,
			StaminaRecoveryData.CoolTime
		);

		OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);	// 블루프린트 디스패처 Call과 같다

		if (CurrentStamina < EmptyCheckLimit)
		{
			OnStaminaEmpty.Broadcast();
		}

		bResult = true;
	}

	//UE_LOG(LogTemp, Log, TEXT("Stamina : %.1f / %.1f"), CurrentStamina, MaxStamina);
	return bResult;
}

void UStatComponent::StaminaAutoRecovertyPerTick()
{
	IStaminaInterface::Execute_RecoveryStamina(this, StaminaRecoveryData.RecoveryPerTick);
}

void UStatComponent::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Min(CurrentStamina + InAmount, MaxStamina);
	OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);
	//UE_LOG(LogTemp, Log, TEXT("Stamina : %.1f / %.1f"), CurrentStamina, MaxStamina);

	//FMath::IsNearlyEqual(CurrentStamina, MaxStamina)
	if (CurrentStamina >= MaxStamina)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(StaminaAutoRecoveryTimerHandle);
	}
}

float UStatComponent::GetCurrentHealth_Implementation() const
{
	return CurrentHealth;
}

float UStatComponent::GetMaxHealth_Implementation() const
{
	return MaxHealth;
}

void UStatComponent::DamageHealth_Implementation(float InAmount)
{
	CurrentHealth -= InAmount;
	if (CurrentHealth < 0.0f )
	{
		CurrentHealth = 0;
		OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
		OnDie.Broadcast();
	}
	else
	{
		OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
	}

	//UE_LOG(LogTemp, Log, TEXT("Health : %.1f / %.1f"), CurrentHealth, MaxHealth);
}

void UStatComponent::HealHealth_Implementation(float InAmount)
{
	CurrentHealth = FMath::Min(CurrentHealth + InAmount, MaxHealth);
	OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
	//UE_LOG(LogTemp, Log, TEXT("Health : %.1f / %.1f"), CurrentHealth, MaxHealth);
}

// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
	//GetCurrentStamina();	// 실행했을 때 C++에 구현된 내용만 호출한다.
	//IStaminaInterface::Execute_GetCurrentStamina(this);	// 실행했을 때 블루프린트 구현으로 호출한다.	
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}




//// 타이머로 대체해서 더 이상 안 씀
//void UStatComponent::StaminaAutoRecoverty(float DeltaTime)
//{
//	StaminaAutoRecoverySecond -= DeltaTime;
//	if (StaminaAutoRecoverySecond < 0.0f)
//	{
//		IStaminaInterface::Execute_RecoveryStamina(this, StaminaAutoRecoveryPerSec * DeltaTime);
//	}
//}