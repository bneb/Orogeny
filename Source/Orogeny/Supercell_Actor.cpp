// Copyright Orogeny. All Rights Reserved.

#include "Supercell_Actor.h"
#include "Orogeny.h"
#include "Components/SphereComponent.h"
#include "Components/VolumetricCloudComponent.h"

// ============================================================================
// Constructor
// ============================================================================

ASupercell_Actor::ASupercell_Actor()
{
	PrimaryActorTick.bCanEverTick = true;

	// -----------------------------------------------------------------------
	// Storm Core — massive collision sphere (200m radius)
	// -----------------------------------------------------------------------
	StormCore = CreateDefaultSubobject<USphereComponent>(TEXT("StormCore"));
	StormCore->InitSphereRadius(DEFAULT_STORM_CORE_RADIUS);
	StormCore->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetRootComponent(StormCore);

	// -----------------------------------------------------------------------
	// Volumetric Cloud Visuals — Tech Artist styles in Blueprint
	// -----------------------------------------------------------------------
	CloudVisuals = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("CloudVisuals"));
	CloudVisuals->SetupAttachment(StormCore);

	UE_LOG(LogOrogeny, Log,
		TEXT("ASupercell_Actor constructed: Speed=%.0f, CoreRadius=%.0f"),
		MovementSpeed, DEFAULT_STORM_CORE_RADIUS);
}

// ============================================================================
// Tracking Interpolation — The Critical Math
// ============================================================================
// This function MUST be pure and deterministic.
// It MUST prevent overshoot/jitter when the storm is very close to target.
//
// PROOF TABLE (Speed=50, DeltaTime=1.0):
//   Current=(0,0,0), Target=(1000,0,0)  → Step=50, Dist=1000 → (50,0,0) ✓
//   Current=(0,0,0), Target=(100,100,0) → Step=50, Dist=141  → diagonal ✓
//   Current=(0,0,0), Target=(10,0,0)    → Step=50, Dist=10   → (10,0,0) ✓ SNAP
// ============================================================================

FVector ASupercell_Actor::CalculateNextPosition(
	const FVector& CurrentLocation, const FVector& TargetLocation,
	float Speed, float DeltaTime)
{
	const FVector ToTarget = TargetLocation - CurrentLocation;
	const double Distance = ToTarget.Size();

	// Guard: already at target (prevent NaN from normalizing zero vector)
	if (Distance < UE_KINDA_SMALL_NUMBER)
	{
		return TargetLocation;
	}

	const double StepSize = static_cast<double>(Speed) * static_cast<double>(DeltaTime);

	// Anti-jitter: if we'd overshoot, snap exactly to target
	if (Distance <= StepSize)
	{
		return TargetLocation;
	}

	// Normal movement: direction * step
	const FVector Direction = ToTarget / Distance; // Manual normalize (we already have length)
	return CurrentLocation + Direction * StepSize;
}

// ============================================================================
// WeakenStorm — Day 9: Combat Damage from Orographic Lift
// ============================================================================
// Both values are floor-clamped:
//   MovementSpeed >= 10.0 (storm never fully stops)
//   CloudOpacity  >= 0.2  (storm never fully disappears)
// ============================================================================

void ASupercell_Actor::WeakenStorm(float SpeedReduction, float OpacityReduction)
{
	MovementSpeed = FMath::Max(MovementSpeed - SpeedReduction, MIN_MOVEMENT_SPEED);
	CloudOpacity = FMath::Max(CloudOpacity - OpacityReduction, MIN_CLOUD_OPACITY);

	UE_LOG(LogOrogeny, Log,
		TEXT("Supercell weakened: Speed=%.1f (reduced %.1f), Opacity=%.2f (reduced %.2f)"),
		MovementSpeed, SpeedReduction, CloudOpacity, OpacityReduction);
}

// ============================================================================
// Tick — Track the Target
// ============================================================================

void ASupercell_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetActor)
	{
		return;
	}

	const FVector NextPosition = CalculateNextPosition(
		GetActorLocation(),
		TargetActor->GetActorLocation(),
		MovementSpeed,
		DeltaTime);

	SetActorLocation(NextPosition);
}
