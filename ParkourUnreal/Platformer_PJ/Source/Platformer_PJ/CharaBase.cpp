// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharaBase.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "PJAnimInstance.h"

//////////////////////////////////////////////////////////////////////////
// APlatformer_PJCharacter

ACharaBase::ACharaBase()
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
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

void ACharaBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharaBase::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharaBase::EndJump);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharaBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharaBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharaBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharaBase::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharaBase::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharaBase::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACharaBase::OnResetVR);
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

void ACharaBase::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}
void ACharaBase::Tick(float DeltaSeconds)
{
	UpdateMovement(DeltaSeconds);
}

void ACharaBase::UpdateMovement(float _delta)
{
	auto movement_component = GetMovementComponent();
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
