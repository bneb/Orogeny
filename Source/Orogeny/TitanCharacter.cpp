// Copyright Orogeny. All Rights Reserved.

#include "TitanCharacter.h"
#include "Orogeny.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TectonicMovementComponent.h"

// Static bone name constants for IK foot placement (Day 2)
const FName ATitanCharacter::LeftFootBoneName = FName(TEXT("foot_l"));
const FName ATitanCharacter::RightFootBoneName = FName(TEXT("foot_r"));

// ============================================================================
// FObjectInitializer Constructor — Injects UTectonicMovementComponent
// ============================================================================
// This is the critical pattern that REPLACES the default
// UCharacterMovementComponent with our custom tectonic version.
// ACharacter creates its movement component during construction,
// and SetDefaultSubobjectClass redirects it to our subclass.
// ============================================================================

ATitanCharacter::ATitanCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTectonicMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// -----------------------------------------------------------------------
	// Capsule — Titan-scale proportions
	// -----------------------------------------------------------------------
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// Don't rotate the character mesh when the camera rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// -----------------------------------------------------------------------
	// Character Movement — ALL defaults now owned by UTectonicMovementComponent
	// -----------------------------------------------------------------------
	// MaxWalkSpeed, MaxAcceleration, BrakingDeceleration, Mass, RotationRate,
	// bOrientRotationToMovement, GravityScale are ALL set in the component's
	// constructor. Do NOT override them here — single source of truth.

	// -----------------------------------------------------------------------
	// Camera Boom — Day 5: Extreme distance for colossal scale
	// -----------------------------------------------------------------------
	// TargetArmLength = 3500 — massively pulled back, Titan is <15% of frame
	// SocketOffset.Z = 800 — looking down from the clouds
	// CameraLagSpeed = 1.5 — very slow follow, simulates heavy drone
	// CameraRotationLagSpeed = 2.0 — sluggish rotation follow
	// -----------------------------------------------------------------------
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 3500.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeSize = 24.0f;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 1.5f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 2.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 800.0f);

	// -----------------------------------------------------------------------
	// Follow Camera
	// -----------------------------------------------------------------------
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	// Wide FOV for landscape vistas
	FollowCamera->FieldOfView = 75.0f;

	UE_LOG(LogOrogeny, Log, TEXT("ATitanCharacter constructed. ArmLength=%.0f, CameraLag=%.1f, SocketOffsetZ=%.0f"),
		CameraBoom->TargetArmLength,
		CameraBoom->CameraLagSpeed,
		CameraBoom->SocketOffset.Z);
}

void ATitanCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void ATitanCharacter::SetIsCommitted(bool bInCommitted)
{
	bIsCommitted = bInCommitted;

	UE_LOG(LogOrogeny, Log, TEXT("ATitanCharacter::SetIsCommitted(%s)"),
		bIsCommitted ? TEXT("TRUE — input locked") : TEXT("FALSE — input unlocked"));
}

void ATitanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Day 5: Footstep-driven camera shake triggering
	// Day 7: Velocity-linked Niagara particle spawn rate
}

void ATitanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATitanCharacter::Move);
		}

		// Looking
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATitanCharacter::Look);
		}
	}
}

void ATitanCharacter::Move(const FInputActionValue& Value)
{
	// Day 4: Check bIsCommitted — if true, ignore all movement input
	if (bIsCommitted)
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATitanCharacter::Look(const FInputActionValue& Value)
{
	// Day 4: When committed, block ALL input including camera
	if (bIsCommitted)
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
