// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ActionCommit.generated.h"

/**
 * UAnimNotifyState_ActionCommit
 *
 * Animation Notify State that locks the Titan into an un-cancelable action.
 * Place this on any Animation Montage to block ALL player input for its duration.
 *
 * Usage:
 *   1. Open an Animation Montage (e.g., "Heavy Stomp") in the Editor
 *   2. Right-click the Notifies track → Add Notify State → ActionCommit
 *   3. Drag the notify to span the frames where input should be blocked
 *   4. Play the montage — the player cannot move or look during those frames
 *
 * Safety:
 *   NotifyInterrupted() is implemented as a failsafe. If the animation is
 *   forcefully interrupted (e.g., by death, stagger, or system override),
 *   bIsCommitted is cleared to prevent permanent input soft-lock.
 *
 * TDD: The lifecycle (Begin/End/Interrupted) is tested headlessly in
 *   ActionStateTest.cpp without needing to play actual animations.
 */
UCLASS(DisplayName = "Action Commit (Input Lock)")
class OROGENY_API UAnimNotifyState_ActionCommit : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyState_ActionCommit();

	// -----------------------------------------------------------------------
	// UAnimNotifyState Interface
	// -----------------------------------------------------------------------

	/**
	 * Called when the notify begins on the montage timeline.
	 * Locks input by setting bIsCommitted = true on the owning ATitanCharacter.
	 */
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference
	) override;

	/**
	 * Called when the notify ends on the montage timeline.
	 * Unlocks input by setting bIsCommitted = false.
	 */
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

	/**
	 * CRITICAL SAFETY: Called if the animation is forcefully interrupted.
	 * Ensures bIsCommitted = false to prevent permanent input soft-lock.
	 * Without this, a killed/staggered titan could lose input forever.
	 */
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation
	);

	// -----------------------------------------------------------------------
	// Display
	// -----------------------------------------------------------------------

	virtual FString GetNotifyName_Implementation() const override;

	// -----------------------------------------------------------------------
	// Helper (used by TDD tests to call notify logic on arbitrary characters)
	// -----------------------------------------------------------------------

	/**
	 * Performs the commit/uncommit action on a given character.
	 * This is the core logic extracted for testability.
	 *
	 * @param Character  The ATitanCharacter to lock/unlock
	 * @param bCommit    True to lock input, false to unlock
	 */
	static void ApplyCommitState(class ATitanCharacter* Character, bool bCommit);
};
