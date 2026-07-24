// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "StatComponent.h"
#include "../Interface/StaminaInterface.h"
#include "../Interface/HealthInterface.h"
#include "../Interface/StatInterface.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
	SphereCollision->InitSphereRadius(100.0f);
	SetRootComponent(SphereCollision);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphereCollision);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	ApplyEffects(OtherActor);
}

void APickup::ApplyEffects(AActor* InTarget)
{
	// bImplements이 true면 인터페이스를 구현했다.
	// bool bImplements = OtherActor->Implements<UStaminaInterface>()
	
	if (IStatInterface* Stat = Cast<IStatInterface>(InTarget))
	{
		UStatComponent* StatComp = Stat->GetStatComponent();
		if (Stamina > 0)
		{
			IStaminaInterface::Execute_RecoveryStamina(StatComp, Stamina);
		}
		else if(Stamina < 0)
		{
			IStaminaInterface::Execute_ConsumeStamina(StatComp, -Stamina);
		}

		if (Health > 0)
		{
			IHealthInterface::Execute_HealHealth(StatComp, Health);
		}
		else if (Health < 0)
		{
			IHealthInterface::Execute_DamageHealth(StatComp, -Health);
		}
	}

	// Target이 null이 아니면 인터페이스를 상속받았다(= C++니까 구현도 되어 있다. 블루프린트에서 상속을 했을 경우는 체크 불가능)
	// IStaminaInterface* Target = Cast<IStaminaInterface>(OtherActor);
}

