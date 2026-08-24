// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interface/StatInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "Interface/InventoryUserInterface.h"
#include "ActionCharacter.generated.h"

class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimNotifyState_SectionJump;
class AActionHUD;

UCLASS()
class UNREAL10TH_CPP_API AActionCharacter 
	: public ACharacter, public IStatInterface, public IWeaponUserInterface, public IInventoryUserInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionCharacter();	
		
	// 이벤트 함수

	// Getter / Setter들
	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual UStatComponent* GetStatComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual UWeaponComponent* GetWeaponComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual UInventoryComponent* GetInventoryComponent() const override;

	// WeaponComponent로 전달할 함수들 ---------------------------------------------------------------
	// 무기 장비 관련 함수들
	virtual void EquipWeapon_Implementation(const UWeaponDataAsset* InWeaponData) override;
	//void UnEquipWeapon();
	//-------------------------------------------------------------------------------------------------

	// InventoryComponent로 전달할 함수들 -------------------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool ExecuteInventoryCommand(const FInventoryCommand& Command, FInventoryCommandResult& OutResult) override;
	// -----------------------------------------------------------------------------------------------
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	void OnTestAction(const FInputActionValue& Value);
	void OnInventoryAction(const FInputActionValue& Value);
	void OnMoveAction(const FInputActionValue& Value);
	void OnAttackAction(const FInputActionValue& Value);
	void OnRollAction(const FInputActionValue& Value);
	void OnSprintStart();
	void OnSprintEnd();

private:
	void SpendSprintStamina(float DeltaTime);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Test;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputAction> IA_Roll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Anims")
	TObjectPtr<UAnimMontage> RollMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float SprintSpeed = 1200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	float WalkSpeed = 600;

	// 구르기에 필요한 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float RollStaminaCost = 30.0f;

	// 달리기에 필요한 초당 스태미너 코스트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float SprintStaminaCostPerSec = 2.0f;

	// 스태미너 사용 후 자동 회복에 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryCoolTime = 3.0f;

	// DEPRECATED : 스태미너가 자동 회복 될 때 초당 회복량
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	//float StaminaAutoRecoveryPerSec = 10.0f;

	// 스태미너가 자동 회복 될 때 타이머 틱당 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryPerTick = 1.0f;

	// 스태미너가 자동 회복 될 때 타이머 한 틱의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat|Stamina")
	float StaminaAutoRecoveryInterval = 0.1f;

	// 공격시 소비되는 스테미너 양
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackCost = 5.0f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStatComponent> StatComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWeaponComponent> WeaponComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInventoryComponent> InvenComponent = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance = nullptr;

	bool bSprintMode = false;

	TWeakObjectPtr<AActionHUD> HUD = nullptr;
};
