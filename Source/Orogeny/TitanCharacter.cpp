// Copyright Orogeny. All Rights Reserved.

#include "TitanCharacter.h"
#include "Orogeny.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Static bone name constants for IK foot placement (Day 2)
const FName ATitanCharacter::LeftFootBoneName = FName(TEXT("foot_l"));
const FName ATitanCharacter::RightFootBoneName = FName(TEXT("foot_r"));

ATitanCharacter::ATitanCharacter()
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
	// Character Movement — Tectonic defaults (Day 3 will subclass this)
	// -----------------------------------------------------------------------
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 200.0f, 0.0f);
	// Day 3 overrides: MaxWalkSpeed=150, BrakingDecelerationWalking=10
	// For now, use reduced defaults to hint at the intended heavy feel
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 10.0f;
	GetCharacterMovement()->GravityScale = 1.5f;

	// -----------------------------------------------------------------------
	// Camera Boom — Extreme distance for colossal scale
	// -----------------------------------------------------------------------
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2500.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeSize = 24.0f;
	// Slight lag for cinematic weight
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 3.0f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 4.0f;
	// Offset upward to frame the titan from below (emphasizes scale)
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 200.0f);

	// -----------------------------------------------------------------------
	// Follow Camera
	// -----------------------------------------------------------------------
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	// Wide FOV for landscape vistas
	FollowCamera->FieldOfView = 75.0f;

	UE_LOG(LogOrogeny, Log, TEXT("ATitanCharacter constructed. ArmLength=%.0f, MaxWalkSpeed=%.0f, Braking=%.0f"),
		CameraBoom->TargetArmLength,
		GetCharacterMovement()->MaxWalkSpeed,
		GetCharacterMovement()->BrakingDecelerationWalking);
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

void ATitanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Day 3: Root motion blending and velocity tracking
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
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
