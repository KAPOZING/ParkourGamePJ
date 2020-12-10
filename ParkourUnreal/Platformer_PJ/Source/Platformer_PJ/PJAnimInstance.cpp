// Copyright Epic Games, Inc. All Rights Reserved.

#include "PJAnimInstance.h"
#include "CharaBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPJAnimInstance::NativeUpdateAnimation(float _delta)
{
	Super::NativeUpdateAnimation(_delta);

	if (IsValid(_OwnerChara))
	{
		auto movement_component = _OwnerChara->GetMovementComponent();
		if (IsValid(movement_component))
		{
			IsFalling = movement_component->IsFalling();
			IsJumping = movement_component->Velocity.Z >= 5.0f;
		}
	}
}

void UPJAnimInstance::BindChara(ACharaBase* _owner)
{
	if (IsValid(_owner))
	{
		_OwnerChara = _owner;
	}
}
