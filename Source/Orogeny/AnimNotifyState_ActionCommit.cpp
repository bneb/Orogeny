// Copyright Orogeny. All Rights Reserved.

#include "AnimNotifyState_ActionCommit.h"
#include "TitanCharacter.h"
#include "Orogeny.h"
#include "Components/SkeletalMeshComponent.h"

// ============================================================================
// Construction
// ============================================================================

UAnimNotifyState_ActionCommit::UAnimNotifyState_ActionCommit()
{
	// Notify can fire in the editor preview — we want it active everywhere
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

// ============================================================================
// Notify Lifecycle
// ============================================================================

void UAnimNotifyState_ActionCommit::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	ATitanCharacter* Titan = Cast<ATitanCharacter>(MeshComp->GetOwner());
	ApplyCommitState(Titan, true);

	if (Titan)
	{
		UE_LOG(LogOrogeny, Log, TEXT("ActionCommit::NotifyBegin — Input LOCKED for %.2f seconds"),
			TotalDuration);
	}
}

void UAnimNotifyState_ActionCommit::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	ATitanCharacter* Titan = Cast<ATitanCharacter>(MeshComp->GetOwner());
	ApplyCommitState(Titan, false);

	UE_LOG(LogOrogeny, Log, TEXT("ActionCommit::NotifyEnd — Input UNLOCKED"));
}

// Overload without EventReference — serves as the interrupt/legacy safety catch
void UAnimNotifyState_ActionCommit::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	ATitanCharacter* Titan = Cast<ATitanCharacter>(MeshComp->GetOwner());
	ApplyCommitState(Titan, false);

	UE_LOG(LogOrogeny, Warning, TEXT("ActionCommit::NotifyEnd (interrupted/legacy) — Input UNLOCKED (safety catch)"));
}

// ============================================================================
// Display Name
// ============================================================================

FString UAnimNotifyState_ActionCommit::GetNotifyName_Implementation() const
{
	return FString(TEXT("Action Commit (Input Lock)"));
}

// ============================================================================
// Core Logic — Static, testable
// ============================================================================

void UAnimNotifyState_ActionCommit::ApplyCommitState(ATitanCharacter* Character, bool bCommit)
{
	if (Character)
	{
		Character->SetIsCommitted(bCommit);
	}
}
