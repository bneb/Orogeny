// Copyright Orogeny. All Rights Reserved.

#include "AnimNotify_TectonicStep.h"
#include "TectonicFootstepShake.h"
#include "TitanCharacter.h"
#include "Orogeny.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ForceFeedbackEffect.h"

// ============================================================================
// Construction
// ============================================================================

UAnimNotify_TectonicStep::UAnimNotify_TectonicStep()
{
	// Notify should fire during gameplay, not in editor preview
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

// ============================================================================
// Notify — Fire Camera Shake + Haptics
// ============================================================================

void UAnimNotify_TectonicStep::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	// -----------------------------------------------------------------------
	// Get the PlayerController (may be null in headless/AI/test scenarios)
	// -----------------------------------------------------------------------
	APlayerController* PC = nullptr;

	if (const APawn* Pawn = Cast<APawn>(Owner))
	{
		PC = Cast<APlayerController>(Pawn->GetController());
	}

	// -----------------------------------------------------------------------
	// Camera Shake — fire if we have a player controller
	// -----------------------------------------------------------------------
	if (PC)
	{
		PC->ClientStartCameraShake(UTectonicFootstepShake::StaticClass(), ShakeScale);

		UE_LOG(LogOrogeny, Verbose, TEXT("TectonicStep: Camera shake fired (scale=%.1f)"), ShakeScale);
	}

	// -----------------------------------------------------------------------
	// Force Feedback / Haptics — fire if effect is set and we have a controller
	// -----------------------------------------------------------------------
	if (PC && ForceFeedbackEffect)
	{
		PC->ClientPlayForceFeedback(ForceFeedbackEffect, false, false, NAME_None);

		UE_LOG(LogOrogeny, Verbose, TEXT("TectonicStep: Force feedback fired"));
	}

	// -----------------------------------------------------------------------
	// Future hooks (Day 6+):
	// - Terrain deformation at foot location
	// - Niagara dust/debris particle burst
	// - MetaSound sub-bass impact
	// -----------------------------------------------------------------------
}

// ============================================================================
// Display Name
// ============================================================================

FString UAnimNotify_TectonicStep::GetNotifyName_Implementation() const
{
	return FString(TEXT("Tectonic Footstep"));
}
