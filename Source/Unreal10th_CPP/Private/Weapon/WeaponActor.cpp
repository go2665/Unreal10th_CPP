// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"
#include "Data/Item/WeaponDataAsset.h"

#include "Unreal10th_CPP/Unreal10th_CPP.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootMesh"));
	SetRootComponent(Mesh);
	// Mesh->SetCollisionProfileName(TEXT("NoCollision"));	// 프로파일을 이용해 한번에 세팅(실제 적용되는 타이밍은 좀 뒤쪽이다)	
	Mesh->SetGenerateOverlapEvents(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	HitArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitArea"));
	HitArea->SetupAttachment(Mesh);
	HitArea->SetCapsuleHalfHeight(60.0f, false);
	HitArea->SetCapsuleRadius(30.0f, false);
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitArea->SetCollisionObjectType(ECC_Weapon);
	HitArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitArea->SetCollisionResponseToChannel(ECC_Enemy, ECR_Overlap);
	HitArea->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailVFX"));
	TrailVFX->SetupAttachment(Mesh);
}

void AWeaponActor::InitializeWeapon(const UWeaponDataAsset* InData)
{
	if (!InData) return;

	WeaponData = InData;
	
	if (WeaponData->IsLoaded())	// 로딩이 완료되었을 때만 처리
	{
		// 에셋 설정
		Mesh->SetSkeletalMesh(WeaponData->Mesh.Get());
		TrailVFX->SetAsset(WeaponData->TrailVFX.Get());
	}
		
	// HitArea크기 조정
	HitArea->SetCapsuleHalfHeight(WeaponData->HitAreaHalfHeight);
	HitArea->SetCapsuleRadius(WeaponData->HitAreaRadius);

	// 사용 회수 설정
	CurrentUseCount = WeaponData->UseCount;
	UE_LOG(LogTemp, Log, TEXT("Current Use Count : %d"), CurrentUseCount);
}

void AWeaponActor::EquipToTarget(AActor* Target)
{
	OnEquipped(Target);	
}

void AWeaponActor::DropWeapon()
{
	if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter))
	{
		if (UWeaponComponent* WeaponComp = WeaponUser->GetWeaponComponent())
		{
			WeaponComp->OnWeaponAttackStateChanged.Clear();
		}
	}

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);

	//Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Ignore);
	Mesh->SetSimulatePhysics(true);
	HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 일정 시간 동안 무기와 플레이어가 충돌 안하게 설정
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(
		PhysicsDelayTimerHandle,
		FTimerDelegate::CreateWeakLambda(
			this,
			[this]()
			{
				Mesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
			}
		),
		PhysicsDelay,
		false
	);

	// 뒤로 던지기
	FVector BackwardDirection = FVector::BackwardVector;
	if (OwnerCharacter.IsValid())
	{
		BackwardDirection = -OwnerCharacter->GetActorForwardVector();
	} 

	FVector ThrowDirection = BackwardDirection * 300.0f + FVector::UpVector * 200.0f;
	Mesh->AddImpulse(ThrowDirection, NAME_None, true);
	FVector AngularImpulse = FVector(
		FMath::RandRange(-200.0f, 200.0f)
	) + GetActorForwardVector() * 1000.0f;
	Mesh->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);

	// DropLifeSpan초 후에 이 액터 제거하기
	SetLifeSpan(DropLifeSpan);

	OnWeaponDrop.Unbind();
	OwnerCharacter = nullptr;
}

void AWeaponActor::Use()
{
	if (WeaponData && !WeaponData->bInfinityUse)
	{
		CurrentUseCount--;
		UE_LOG(LogTemp, Log, TEXT("Current Use Count : %d"), CurrentUseCount);
		if (CurrentUseCount <= 0)
		{
			OnWeaponDrop.ExecuteIfBound(WeaponData);
		}
	}
}

void AWeaponActor::ResetUseCount()
{
	CurrentUseCount = WeaponData->UseCount;
	UE_LOG(LogTemp, Log, TEXT("Current Use Count : %d"), CurrentUseCount);
}

FVector AWeaponActor::GetWeaponImpactLocation() const
{	
	return FMath::Lerp(Mesh->GetSocketLocation(TEXT("Tip")), Mesh->GetSocketLocation(TEXT("Base")), 0.5f);
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	HitArea->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnHitAreaBeginOverlap);
	TrailVFX->Deactivate();
}

void AWeaponActor::OnEquipped(AActor* InOwner)
{
	if (!WeaponData)
	{
		return;
	}

	SetOwner(InOwner);
	OwnerCharacter = Cast<ACharacter>(InOwner);
	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		true);

	if (OwnerCharacter.IsValid())
	{
		AttachToComponent(OwnerCharacter->GetMesh(), AttachRules, WeaponData->AttachSocketName);

		// Offset적용
		SetActorRelativeLocation(WeaponData->LocationOffset);

		HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true);	// 만약을 대비한 것

		if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter))
		{
			if (UWeaponComponent* WeaponComp = WeaponUser->GetWeaponComponent())
			{
				WeaponComp->OnWeaponAttackStateChanged.BindUFunction(this, FName("AttackEnable"));
			}
		}
	}
}

void AWeaponActor::OnHitAreaBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
	if (!OwnerCharacter.IsValid() || !InOtherActor) return;

	float Damage = WeaponData ? WeaponData->AttackPower : 1;	

	UE_LOG(LogTemp, Log, TEXT("오버랩 된 대상 : %s"), *InOtherActor->GetName());
	//UGameplayStatics::ApplyDamage()를 호출하면 대상의 TakeDamage함수가 호출된다.
	UGameplayStatics::ApplyDamage(InOtherActor, Damage, OwnerCharacter->GetController(), this, nullptr);
}

void AWeaponActor::AttackEnable(bool bEnable)
{
	if (bEnable)
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TrailVFX->Activate();
	}
	else
	{
		HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TrailVFX->Deactivate();
	}
}
