// Copyright Epic Games, Inc. All Rights Reserved.

#include "PJAnimInstance.h"
#include "Chara/CharaBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPJAnimInstance::NativeUpdateAnimation(float _delta)
{
	Super::NativeUpdateAnimation(_delta);

	if (IsValid(_OwnerChara))
	{
		IsFalling = _OwnerChara->IsFalling;
		IsJumping = _OwnerChara->IsJumping;
	}
}

void UPJAnimInstance::BindChara(ACharaBase* _owner)
{
	if (IsValid(_owner))
	{
		_OwnerChara = _owner;
	}
}
