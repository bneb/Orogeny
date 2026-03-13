// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ExecuteLift.generated.h"

/**
 * UAnimNotify_ExecuteLift
 *
 * Day 9: Combat Anim Notify — fires the Orographic Lift AOE.
 *
 * Place this on the impact frame of the "Thrust" Animation Montage.
 * When the montage reaches this notify, the Titan's OrographicLiftComponent
 * executes GetLift(), weakening all Supercells within range.
 *
 * SAFETY:
 * - Null-safe for Owner, Character cast, and component lookup.
 * - Works in headless/test scenarios (no crash on null mesh owner).
 */
UCLASS(meta = (DisplayName = "Execute Orographic Lift"))
class OROGENY_API UAnimNotify_ExecuteLift : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
