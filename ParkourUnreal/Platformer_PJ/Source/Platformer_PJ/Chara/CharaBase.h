// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Common/Enum/ControlType.h"
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
	ACharaBase( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsJumping = false;


protected:
	float JumpPressedTime = 0.0f;
	float NormalControlTypeAcceleration = 1024.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EControlType	CurrentControlType = EControlType::Normal;

public:
	UPJAnimInstance* GetAnimInstance() const;

	UFUNCTION(BlueprintCallable)
		void	ChangeControlType(EControlType _type);

protected:
	void JumpByVelocity( float ZVelocity);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateMovement(float _delta);
	void UpdateRotationRate( float _delta);

	void StartJump();
	void EndJump();


	// Input Event
	void INPUT_OnAxis_MoveForward( float _value);
	void INPUT_OnAxis_MoveRight(float _value);

	void INPUT_OnPressed_Jump();
	void INPUT_OnReleased_Jump();
	void INPUT_OnPressed_AccelRunning();
	void INPUT_OnReleased_AccelRunning();


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

	virtual bool CanJumpInternal_Implementation() const override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

