// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_TectonicStep.generated.h"

class UForceFeedbackEffect;
class USoundBase;

/**
 * UAnimNotify_TectonicStep
 *
 * Animation Notify fired on each Titan footfall. Triggers:
 *   1. UTectonicFootstepShake — procedural camera shake
 *   2. UForceFeedbackEffect — controller haptics (optional, set in Editor)
 *
 * USAGE:
 *   1. Open the Titan's walk/run Animation Montage
 *   2. Right-click the Notifies track → Add Notify → TectonicStep
 *   3. Place one notify on each foot-down keyframe
 *   4. (Optional) Set the ForceFeedback property in the Details panel
 *
 * DESIGN:
 *   The camera shake fires every time the foot hits the ground.
 *   At 150 cm/s max walk speed with ~1.5s stride cycle, that's
 *   roughly one shake every 0.75s per foot — intentionally overlapping
 *   to create a continuous gentle rumble.
 *
 * TDD: Null safety verified in CameraScaleTest.cpp (no crash without controller)
 */
UCLASS(DisplayName = "Tectonic Footstep")
class OROGENY_API UAnimNotify_TectonicStep : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_TectonicStep();

	// -----------------------------------------------------------------------
	// UAnimNotify Interface
	// -----------------------------------------------------------------------

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

	virtual FString GetNotifyName_Implementation() const override;

	// -----------------------------------------------------------------------
	// Configurable Properties
	// -----------------------------------------------------------------------

	/**
	 * Optional force feedback effect for controller haptics.
	 * Set this in the Editor's Details panel on the notify instance.
	 * If null, only camera shake fires (no haptics).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Haptics")
	TObjectPtr<UForceFeedbackEffect> ForceFeedbackEffect;

	/**
	 * Scale multiplier for the camera shake intensity.
	 * 1.0 = default, <1.0 = softer (toe placement), >1.0 = harder (stomp).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Camera", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float ShakeScale = 1.0f;

	/**
	 * Sound to play on each footstep impact (e.g., MetaSound sub-bass thud).
	 * Played at the mesh component's world location via PlaySoundAtLocation.
	 * If null, no footstep audio fires (camera shake + haptics still work).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Audio")
	TObjectPtr<USoundBase> FootstepSound;
};
