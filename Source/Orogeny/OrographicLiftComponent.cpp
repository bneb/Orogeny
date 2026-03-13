// Copyright Orogeny. All Rights Reserved.

#include "OrographicLiftComponent.h"
#include "Orogeny.h"
#include "Supercell_Actor.h"
#include "Engine/OverlapResult.h"

// ============================================================================
// Constructor
// ============================================================================

UOrographicLiftComponent::UOrographicLiftComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Event-driven, not tick-driven

	UE_LOG(LogOrogeny, Log,
		TEXT("UOrographicLiftComponent constructed: Radius=%.0f, SpeedDmg=%.1f, OpacityDmg=%.2f"),
		LiftRadius, SpeedDamage, OpacityDamage);
}

// ============================================================================
// ExecuteLift — The AOE Attack
// ============================================================================
// 1. Sphere overlap from owner's world location
// 2. Find all ASupercell_Actor instances within LiftRadius
// 3. Call WeakenStorm on each
// ============================================================================

void UOrographicLiftComponent::ExecuteLift()
{
	AActor* Owner = GetOwner();
	if (!Owner || !GetWorld())
	{
		return;
	}

	const FVector Origin = Owner->GetActorLocation();

	// -----------------------------------------------------------------------
	// Sphere overlap query
	// -----------------------------------------------------------------------
	FCollisionObjectQueryParams ObjectQuery;
	ObjectQuery.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(LiftRadius);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(
		Overlaps, Origin, FQuat::Identity, ObjectQuery, SphereShape);

	// -----------------------------------------------------------------------
	// Find and weaken all Supercells
	// -----------------------------------------------------------------------
	int32 HitCount = 0;
	for (const FOverlapResult& Overlap : Overlaps)
	{
		ASupercell_Actor* Supercell = Cast<ASupercell_Actor>(Overlap.GetActor());
		if (Supercell)
		{
			Supercell->WeakenStorm(SpeedDamage, OpacityDamage);
			HitCount++;
		}
	}

	UE_LOG(LogOrogeny, Log,
		TEXT("Orographic Lift executed: Origin=(%.0f, %.0f, %.0f), Radius=%.0f, Hits=%d"),
		Origin.X, Origin.Y, Origin.Z, LiftRadius, HitCount);
}
