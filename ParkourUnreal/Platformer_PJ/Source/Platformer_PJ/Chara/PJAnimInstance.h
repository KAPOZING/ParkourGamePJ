// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "PJAnimInstance.generated.h"

class ACharaBase;

UCLASS()
class UPJAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	
		ACharaBase* _OwnerChara = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool	IsJumping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool	IsFalling = false;

public:
	UFUNCTION(BlueprintCallable)
		void BindChara(ACharaBase* _owner);



public:
	virtual void NativeUpdateAnimation(float _delta) override;
};

