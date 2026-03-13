// Copyright Orogeny. All Rights Reserved.

#include "AnimNotify_ExecuteLift.h"
#include "Orogeny.h"
#include "OrographicLiftComponent.h"

void UAnimNotify_ExecuteLift::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	UOrographicLiftComponent* LiftComp =
		MeshComp->GetOwner()->FindComponentByClass<UOrographicLiftComponent>();

	if (LiftComp)
	{
		LiftComp->ExecuteLift();
		UE_LOG(LogOrogeny, Log, TEXT("AnimNotify_ExecuteLift: Lift dispatched."));
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("AnimNotify_ExecuteLift: No UOrographicLiftComponent found on %s"),
			*MeshComp->GetOwner()->GetName());
	}
}
