// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharaBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"


void ACharaBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharaBase::INPUT_OnPressed_Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharaBase::INPUT_OnReleased_Jump);

	PlayerInputComponent->BindAction("AccelRunning", IE_Pressed, this, &ACharaBase::INPUT_OnPressed_AccelRunning);
	PlayerInputComponent->BindAction("AccelRunning", IE_Released, this, &ACharaBase::INPUT_OnReleased_AccelRunning);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharaBase::INPUT_OnAxis_MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharaBase::INPUT_OnAxis_MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharaBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharaBase::LookUpAtRate);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharaBase::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharaBase::TouchStopped);
}

void ACharaBase::INPUT_OnAxis_MoveForward(float _value)
{
	InputVelocity.X = _value;
	// MoveForward(_value);
}

void ACharaBase::INPUT_OnAxis_MoveRight(float _value)
{
	InputVelocity.Y = _value;
}

void ACharaBase::INPUT_OnPressed_Jump()
{
	StartJump();
}

void ACharaBase::INPUT_OnReleased_Jump()
{
	EndJump();
}

void ACharaBase::INPUT_OnPressed_AccelRunning()
{
	ChangeControlType(EControlType::AccelRunning);
}

void ACharaBase::INPUT_OnReleased_AccelRunning()
{
	ChangeControlType(EControlType::Normal);
}