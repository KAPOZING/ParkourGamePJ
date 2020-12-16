// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharaBase.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Chara/PJCharaMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "PJAnimInstance.h"

//////////////////////////////////////////////////////////////////////////
// APlatformer_PJCharacter

ACharaBase::ACharaBase(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer.SetDefaultSubobjectClass<UPJCharaMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void ACharaBase::BeginPlay()
{
	Super::BeginPlay();

	auto anim_instance = GetAnimInstance();

	if (IsValid(anim_instance))
	{
		anim_instance->BindChara(this);
	}
}

void ACharaBase::ChangeControlType(EControlType _type)
{
	CurrentControlType = _type;

	auto movement_component = Cast<UPJCharaMovementComponent>( GetMovementComponent());
	if (IsValid(movement_component))
	{
		switch (CurrentControlType)
		{
		case EControlType::Normal:
			movement_component->MaxAcceleration = NormalControlTypeAcceleration;
			break;

		case EControlType::AccelRunning:
			movement_component->MaxAcceleration = 100000.0f;
			break;
		}
	}
}

UPJAnimInstance* ACharaBase::GetAnimInstance() const
{
	auto mesh = GetMesh();

	if (IsValid(mesh))
	{
		return Cast<UPJAnimInstance>(mesh->GetAnimInstance());
	}

	return nullptr;
}

void ACharaBase::StartJump()
{
	JumpPressedTime = 0.0f;
	IsJumpPressed = true;
}

void ACharaBase::EndJump()
{
	float velocity = ShortJumpVelocity;
	if (JumpPressedTime >= LongJumpPressedTime)
	{
		velocity = LongJumpVelocity;
	}

	JumpByVelocity(velocity);

	IsJumpPressed = false;
}

void ACharaBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateMovement(DeltaSeconds);
}

void ACharaBase::UpdateMovement(float _delta)
{
	auto movement_component = GetCharacterMovement();
	if (IsValid(movement_component))
	{
		float velocity_z = movement_component->Velocity.Z;
		IsFalling = movement_component->IsFalling();
		IsJumping = velocity_z >= 5.0f;
	}

	if (IsJumpPressed)
	{
		JumpPressedTime += _delta;
		if (JumpPressedTime >= LongJumpPressedTime)
		{
			EndJump();
		}
	}

	if (CurrentControlType == EControlType::AccelRunning)
	{
		AddMovementInput(GetActorForwardVector(), 1.0f, true);

		if (!FMath::IsNearlyZero(InputVelocity.Y, FLT_EPSILON))
		{
			auto rotator = GetActorRotation();

			rotator.Yaw += WalkRotateRate * 2.0f *  InputVelocity.Y * _delta;

			SetActorRotation(rotator);

		}
	}
	else if (!InputVelocity.IsNearlyZero())
	{
		auto current_rot = GetActorRotation();
		auto current_quat = GetActorRotation().Quaternion();

		FRotator controller_rotator = GetControlRotation();
		controller_rotator.Pitch = 0.0f;
		controller_rotator.Roll = 0.0f;

		FVector controller_axis_input = controller_rotator.RotateVector(InputVelocity);

		FRotator dest_rot = controller_axis_input.Rotation();
		auto dest_quat = dest_rot.Quaternion();

		float yaw_dist = dest_rot.Yaw - current_rot.Yaw;

		// dist Factor Calc
		float dist_factor = FMath::Clamp((FMath::Abs(((int)yaw_dist % 360 - 180.0f) / 180.0f)), 0.1f, 1.0f);

		float rotate_rate = WalkRotateRate;

		if (IsValid(movement_component))
		{
			rotate_rate = movement_component->RotationRate.Yaw;
		}

		SetActorRotation(FQuat::Slerp(current_quat, dest_quat, _delta * dist_factor * rotate_rate * 0.03f).Rotator());

		AddMovementInput(GetActorForwardVector(), 1.0f, true);
	}

	UpdateRotationRate(_delta);
}

bool ACharaBase::CanJumpInternal_Implementation() const
{
	if (IsJumping || IsFalling)
		return false;

	return Super::CanJumpInternal_Implementation();
}

void ACharaBase::UpdateRotationRate(float _delta)
{
	FVector velocity = GetVelocity();

	float rotation_rate = StandRotateRate;
	if (  velocity.Size() >= 200.0f)
	{
		rotation_rate = WalkRotateRate;
	}

	auto movement_component = GetCharacterMovement();
	if (IsValid(movement_component))
	{
		movement_component->RotationRate = FRotator(0, rotation_rate, 0);
	}
}

void ACharaBase::JumpByVelocity(float ZVelocity)
{
	auto movement_component = GetCharacterMovement();

	if (IsValid(movement_component))
	{
		movement_component->JumpZVelocity = ZVelocity;
	}

	Jump();
}


void ACharaBase::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	StartJump();
}

void ACharaBase::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	EndJump();
}

void ACharaBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharaBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ACharaBase::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharaBase::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
