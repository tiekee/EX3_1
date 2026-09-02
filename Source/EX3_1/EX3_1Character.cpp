// Copyright Epic Games, Inc. All Rights Reserved.

#include "EX3_1Character.h"

#include "EX3_1.h"
#include "EX3_1GameMode.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"

AEX3_1Character::AEX3_1Character()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void AEX3_1Character::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	BP_OnHealthChanged(CurrentHealth, MaxHealth);
}

void AEX3_1Character::AddCoins(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	CoinCount += Amount;
	BP_OnCoinCountChanged(CoinCount);
	UE_LOG(LogEX3_1, Log, TEXT("%s collected %d coin(s). Total: %d"), *GetName(), Amount, CoinCount);
}

void AEX3_1Character::Heal(float Amount)
{
	if (Amount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
	if (!FMath::IsNearlyEqual(PreviousHealth, CurrentHealth))
	{
		BP_OnHealthChanged(CurrentHealth, MaxHealth);
		UE_LOG(LogEX3_1, Log, TEXT("%s healed %.0f health. Current: %.0f/%.0f"),
			*GetName(), CurrentHealth - PreviousHealth, CurrentHealth, MaxHealth);
	}
}

void AEX3_1Character::RestoreFullHealth()
{
	if (CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth = MaxHealth;
	BP_OnHealthChanged(CurrentHealth, MaxHealth);
}

float AEX3_1Character::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	const float FinalDamage = FMath::Max(AppliedDamage, DamageAmount);
	if (FinalDamage <= 0.0f || CurrentHealth <= 0.0f)
	{
		return 0.0f;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - FinalDamage, 0.0f, MaxHealth);
	BP_OnHealthChanged(CurrentHealth, MaxHealth);
	UE_LOG(LogEX3_1, Log, TEXT("%s took %.0f damage. Current: %.0f/%.0f"),
		*GetName(), FinalDamage, CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		DisableInput(Cast<APlayerController>(GetController()));
		if (AEX3_1GameMode* GameMode = GetWorld()->GetAuthGameMode<AEX3_1GameMode>())
		{
			GameMode->HandlePlayerDeath(this);
		}
	}

	return FinalDamage;
}

void AEX3_1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEX3_1Character::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AEX3_1Character::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEX3_1Character::Look);
	}
	else
	{
		UE_LOG(LogEX3_1, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}

	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AEX3_1Character::StartSprinting);
	PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &AEX3_1Character::StopSprinting);
	PlayerInputComponent->BindKey(EKeys::RightShift, IE_Pressed, this, &AEX3_1Character::StartSprinting);
	PlayerInputComponent->BindKey(EKeys::RightShift, IE_Released, this, &AEX3_1Character::StopSprinting);
}

void AEX3_1Character::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AEX3_1Character::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AEX3_1Character::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		const FRotator YawRotation(0.0f, GetController()->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Forward);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Right);
	}
}

void AEX3_1Character::DoLook(float Yaw, float Pitch)
{
	if (GetController())
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AEX3_1Character::DoJumpStart()
{
	Jump();
}

void AEX3_1Character::DoJumpEnd()
{
	StopJumping();
}

void AEX3_1Character::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AEX3_1Character::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
