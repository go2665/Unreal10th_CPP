// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

float UStatComponent::GetCurrentStamina_Implementation() const
{
	return CurrentStamina;
}

bool UStatComponent::ConsumeStamina_Implementation(float InAmount)
{
	bool bResult = false;
	if (CurrentStamina >= InAmount)
	{
		CurrentStamina -= InAmount;

		//StaminaAutoRecoverySecond = StaminaAutoRecoveryCoolTime;	// 스테미너를 사용 했으면 StaminaAutoRecoverySecond 리셋

		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(
			StaminaAutoRecoveryTimerHandle,
			this,
			&AActionCharacter::StaminaAutoRecoverty,
			StaminaAutoRecoveryInterval,
			true,
			StaminaAutoRecoveryCoolTime
		);

		bResult = true;
	}

	UE_LOG(LogTemp, Log, TEXT("Stamina : %.1f / %.1f"), CurrentStamina, MaxStamina);
	return bResult;
}

void UStatComponent::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + InAmount, 0.0f, MaxStamina);
	UE_LOG(LogTemp, Log, TEXT("Stamina : %.1f / %.1f"), CurrentStamina, MaxStamina);

	//FMath::IsNearlyEqual(CurrentStamina, MaxStamina)
	if (CurrentStamina >= MaxStamina)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.ClearTimer(StaminaAutoRecoveryTimerHandle);
	}
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
	
}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

