// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TectonicFeedbackComponent.generated.h"

class UCameraShakeBase;
class UForceFeedbackEffect;

/**
 * UTectonicFeedbackComponent
 *
 * Sprint 11: Game Feel — The Titan's Weight
 *
 * Drives Camera Shakes, Force Feedback (controller rumble), and
 * diegetic Post-Process (health vignette) based on the Titan's
 * physical state.
 *
 * ARCHITECTURE:
 *   - Tick reads velocity, subduction state, and ecosystem health.
 *   - FFeedbackMath translates raw state to normalized intensities.
 *   - Intensities drive PlayerController::ClientStartCameraShake,
 *     ClientPlayForceFeedback, and PostProcessVolume adjustments.
 *
 * ASSIGNMENT:
 *   Place this component on the Titan Character BP.
 *   Assign FootstepShakeClass and SubductionShakeClass in Details.
 */
UCLASS(ClassGroup = (Orogeny), meta = (BlueprintSpawnableComponent))
class OROGENY_API UTectonicFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTectonicFeedbackComponent();

	// -----------------------------------------------------------------------
	// Shake Assets (assign in Editor)
	// -----------------------------------------------------------------------

	/** Camera shake for footstep heave (Perlin noise based) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Feedback")
	TSubclassOf<UCameraShakeBase> FootstepShakeClass;

	/** Camera shake for subduction grinding */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Feedback")
	TSubclassOf<UCameraShakeBase> SubductionShakeClass;

	/** Force feedback for subduction controller rumble */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Feedback")
	TObjectPtr<UForceFeedbackEffect> SubductionRumbleAsset;

	// -----------------------------------------------------------------------
	// State (read-only, for debugging)
	// -----------------------------------------------------------------------

	/** Current footstep shake scale [0, 1] */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Feedback|State")
	float CurrentShakeScale = 0.0f;

	/** Current health vignette weight [0, 1] */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Feedback|State")
	float CurrentVignetteWeight = 0.0f;

	/** Current subduction rumble intensity [0, 1] */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Feedback|State")
	float CurrentRumbleIntensity = 0.0f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
};
