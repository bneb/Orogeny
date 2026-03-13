// Copyright Orogeny. All Rights Reserved.

#include "TectonicFeedbackComponent.h"
#include "Orogeny.h"
#include "FeedbackMath.h"
#include "TitanSubductionComponent.h"
#include "EcosystemArmorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraShakeBase.h"

// ============================================================================
// Constructor
// ============================================================================

UTectonicFeedbackComponent::UTectonicFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Feedback at 20Hz — smooth enough for feel, lean for iGPU
	PrimaryComponentTick.TickInterval = 0.05f;
}

// ============================================================================
// BeginPlay
// ============================================================================

void UTectonicFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogOrogeny, Log, TEXT("TectonicFeedbackComponent: Initialized on %s"),
		*GetOwner()->GetName());

	if (!FootstepShakeClass)
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("TectonicFeedbackComponent: No FootstepShakeClass assigned!"));
	}
	if (!SubductionShakeClass)
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("TectonicFeedbackComponent: No SubductionShakeClass assigned!"));
	}
}

// ============================================================================
// TickComponent — Read state → Calculate feedback → Apply
// ============================================================================

void UTectonicFeedbackComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner) return;

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character) return;

	APlayerController* PC = Cast<APlayerController>(Character->GetController());

	// -----------------------------------------------------------------------
	// Walk Shake — velocity → shake scale
	// -----------------------------------------------------------------------
	float VelocityAlpha = 0.0f;
	if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
	{
		const float MaxSpeed = Movement->MaxWalkSpeed;
		if (MaxSpeed > 0.0f)
		{
			VelocityAlpha = FMath::Clamp(
				Character->GetVelocity().Size() / MaxSpeed, 0.0f, 1.0f);
		}
	}

	CurrentShakeScale = FFeedbackMath::CalculateFootstepShakeScale(VelocityAlpha);

	if (PC && FootstepShakeClass && CurrentShakeScale > 0.0f)
	{
		PC->ClientStartCameraShake(FootstepShakeClass, CurrentShakeScale);
	}

	// -----------------------------------------------------------------------
	// Subduction Rumble — depth → controller rumble
	// -----------------------------------------------------------------------
	CurrentRumbleIntensity = 0.0f;
	UTitanSubductionComponent* Subduction =
		Owner->FindComponentByClass<UTitanSubductionComponent>();
	if (Subduction)
	{
		CurrentRumbleIntensity = FFeedbackMath::CalculateSubductionRumbleIntensity(
			Subduction->bIsTransitioning,
			Subduction->bIsSubducted ? Subduction->SubductionDepth : 0.0f,
			Subduction->SubductionDepth);

		if (PC && SubductionShakeClass && CurrentRumbleIntensity > 0.0f)
		{
			PC->ClientStartCameraShake(SubductionShakeClass, CurrentRumbleIntensity);
		}

		if (PC && SubductionRumbleAsset && CurrentRumbleIntensity > 0.0f)
		{
			FForceFeedbackParameters FeedbackParams;
			FeedbackParams.Tag = FName("SubductionRumble");
			FeedbackParams.bLooping = false;
			FeedbackParams.bIgnoreTimeDilation = true;
			FeedbackParams.bPlayWhilePaused = false;
			PC->ClientPlayForceFeedback(SubductionRumbleAsset, FeedbackParams);
		}
	}

	// -----------------------------------------------------------------------
	// Health Vignette — ecosystem health → post-process
	// -----------------------------------------------------------------------
	CurrentVignetteWeight = 0.0f;
	UEcosystemArmorComponent* Ecosystem =
		Owner->FindComponentByClass<UEcosystemArmorComponent>();
	if (Ecosystem)
	{
		CurrentVignetteWeight = FFeedbackMath::CalculateHealthVignetteWeight(
			Ecosystem->EcosystemHealth);
	}

	// Post-process vignette application is handled by a PostProcessVolume
	// bound to the camera, driven by Blueprint or Material Parameter
	// Collection. The C++ provides the normalized weight; visual artists
	// map it to VignetteIntensity and ColorSaturation in the PPV.
}
