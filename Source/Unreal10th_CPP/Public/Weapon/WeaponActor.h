// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

DECLARE_DELEGATE_OneParam(FOnWeaponDrop, const UWeaponDataAsset*);

class ACharacter;
class UCapsuleComponent;
class UNiagaraComponent;

UCLASS()
class UNREAL10TH_CPP_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponActor();

	UFUNCTION(BlueprintCallable)
	void InitializeWeapon(const UWeaponDataAsset* InData);

	UFUNCTION(BlueprintCallable)
	void EquipToTarget(AActor* Target);

	UFUNCTION(BlueprintCallable)
	void DropWeapon();

	UFUNCTION(BlueprintCallable)
	bool CanUse() const { return CurrentUseCount > 0; }

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION(BlueprintCallable)
	void ResetUseCount();

	UFUNCTION(BlueprintCallable)
	FVector GetWeaponImpactLocation() const;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnEquipped(AActor* InOwner);

	UFUNCTION()
	void OnHitAreaBeginOverlap(
		UPrimitiveComponent* InOverlappedComponent,
		AActor* InOtherActor,
		UPrimitiveComponent* InOtherComp,
		int32					InOtherBodyIndex,
		bool					bFromSweep,
		const FHitResult& InSweepResult);

	UFUNCTION(BlueprintCallable)
	void AttackEnable(bool bEnable);

public:
	FOnWeaponDrop OnWeaponDrop;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> HitArea = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> TrailVFX = nullptr;

	// 무기 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<const UWeaponDataAsset> WeaponData;

	// 무기 사용 회수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	int32 CurrentUseCount = 1;

	// 무기가 드랍 된 후 사라질 때까지의 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DropLifeSpan = 10.0f;

	// 드랍 직후에 플레이어와 물리 상호작용이 안되는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PhysicsDelay = 0.8f;

private:
	// 무기를 장비하고 있는 대상
	TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;

	// PhysicsDelay용 타이머 핸들
	FTimerHandle PhysicsDelayTimerHandle;
};
