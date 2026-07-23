// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interface/StaminaInterface.h"
#include "StatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREAL10TH_CPP_API UStatComponent : public UActorComponent, public IStaminaInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

	virtual float GetCurrentStamina_Implementation() const override;
	virtual bool ConsumeStamina_Implementation(float InAmount) override;
	virtual void RecoveryStamina_Implementation(float InAmount) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// DEPRECATED : 타이머로 변경하면서 더 이상 사용하지 않음
	void StaminaAutoRecoverty(float DeltaTime);

private:
	void SpendSprintStamina(float DeltaTime);
	void StaminaAutoRecoverty();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 100.0f;

private:
	// DPRECATED : 틱에서 타이머로 변경되며 사용 안함
	float StaminaAutoRecoverySecond = 0.0f;

	// 스테미너 자동 회복 처리를 위한 타이머
	FTimerHandle StaminaAutoRecoveryTimerHandle;
};
