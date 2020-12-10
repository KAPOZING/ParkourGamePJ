// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharaBase.generated.h"

class UPJAnimInstance;

UCLASS(config=Game)
class ACharaBase : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ACharaBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
		float LongJumpPressedTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
		float ShortJumpVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
		float LongJumpVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
		float StandRotateRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
		float WalkRotateRate;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsJumpPressed = false;

protected:
	float JumpPressedTime = 0.0f;


public:
	UPJAnimInstance* GetAnimInstance() const;


protected:
	void JumpByVelocity( float ZVelocity);

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateMovement(float _delta);
	void UpdateRotationRate( float _delta);

	void StartJump();
	void EndJump();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	virtual void BeginPlay() override;
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

