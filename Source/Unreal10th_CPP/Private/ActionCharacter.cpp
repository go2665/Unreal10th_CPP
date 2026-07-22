// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AActionCharacter::AActionCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComponent->SetupAttachment(RootComponent);
	CameraSpringArmComponent->bUsePawnControlRotation = true;	// 스프링암은 컨트롤러 입력에 맞게 회전되기

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	CameraComponent->SetupAttachment(CameraSpringArmComponent);

	bUseControllerRotationYaw = false;	// 컨트롤러 움직일 때 폰이 같이 회전되는 것 방지
	GetCharacterMovement()->bOrientRotationToMovement = true;	// 캐릭터 이동방향으로 바라보게 만들기
}

float AActionCharacter::GetCurrentStamina_Implementation() const
{
	return CurrentStamina;
}

bool AActionCharacter::ConsumeStamina_Implementation(float InAmount)
{
	bool bResult = false;
	if (CurrentStamina >= InAmount)
	{
		CurrentStamina -= InAmount;
		bResult = true;
	}

	UE_LOG(LogTemp, Log, TEXT("현재 Stamina : %.1f"), CurrentStamina);
	return bResult;
}

void AActionCharacter::RecoveryStamina_Implementation(float InAmount)
{
	CurrentStamina = FMath::Clamp(CurrentStamina + InAmount, 0.0f, MaxStamina);
	UE_LOG(LogTemp, Log, TEXT("현재 Stamina : %.1f"), CurrentStamina);
}

// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}

	CurrentStamina = MaxStamina;

	//GetCurrentStamina();	// 실행했을 때 C++에 구현된 내용만 호출한다.
	//IStaminaInterface::Execute_GetCurrentStamina(this);	// 실행했을 때 블루프린트 구현으로 호출한다.
}

// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &AActionCharacter::OnTestAction);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionCharacter::OnMoveAction);
		EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &AActionCharacter::OnRollAction);
		EnhancedInputComponent->BindActionValueLambda(IA_Sprint, ETriggerEvent::Started,
			[this](const FInputActionValue& _) {
				OnSprintStart();
			});
		EnhancedInputComponent->BindActionValueLambda(IA_Sprint, ETriggerEvent::Completed,
			[this](const FInputActionValue& _) {
				OnSprintEnd();
			});
	}
}

void AActionCharacter::OnTestAction(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("TestAction 실행"));

	//Value.Get<bool>();

}

void AActionCharacter::OnMoveAction(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	FVector WorldDirection = FVector(Input.Y, Input.X, 0).GetSafeNormal();
	
	//UE_LOG(LogTemp, Log, TEXT("Input : %.1f, %.1f"), Input.X, Input.Y);
	//UE_LOG(LogTemp, Log, TEXT("Input : %s"), *Input.ToString());
	//UE_LOG(LogTemp, Log, TEXT("WorldDirection : %.1f, %.1f"), WorldDirection.X, WorldDirection.Y);

	//GetControlRotation(); 컨트롤러의 회전

	// 카메라의 Yaw회전각(Degree)를 Radian으로 변경
	float YawRadian = FMath::DegreesToRadians(GetControlRotation().Yaw);	
	
	// 좌우 회전만 할꺼라 UpVector를 기준축으로 Yaw회전각 만큼 돌리는 회전 만들기
	FQuat ContolYawRotation(FVector::UpVector, YawRadian);					

	// 입력된 방향에 회전 적용(=카메라 Yaw회전 만큼 입력방향을 회전 시키기)
	WorldDirection = ContolYawRotation.RotateVector(WorldDirection);

	AddMovementInput(WorldDirection);
}

void AActionCharacter::OnRollAction(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Log, TEXT("OnRollAction"));
	if (!RollMontage) return;
	
	if (!AnimInstance)
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}

	if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
	{
		if (!GetLastMovementInputVector().IsNearlyZero())	// 이동 입력 중이면
		{
			SetActorRotation(GetLastMovementInputVector().Rotation());	// 입력방향으로 즉시 회전해서 구르기
		}

		PlayAnimMontage(RollMontage);
	}
}

void AActionCharacter::OnSprintStart()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AActionCharacter::OnSprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

