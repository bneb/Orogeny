// Copyright Orogeny. All Rights Reserved.

#include "TitanAnimInstance.h"
#include "Orogeny.h"
#include "TitanCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"

// ============================================================================
// Construction
// ============================================================================

UTitanAnimInstance::UTitanAnimInstance()
{
	// Defaults set in header via member initializers
}

// ============================================================================
// UAnimInstance Interface
// ============================================================================

void UTitanAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// Cache the owning character reference
	OwningCharacter = Cast<ACharacter>(TryGetPawnOwner());

	if (OwningCharacter)
	{
		UE_LOG(LogOrogeny, Log, TEXT("UTitanAnimInstance initialized for %s. TraceDistance=%.0f, MaxIKOffset=%.0f"),
			*OwningCharacter->GetName(), TraceDistance, MaxIKOffset);
	}
}

void UTitanAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// Guard: need a valid character and non-zero delta
	if (!OwningCharacter || DeltaSeconds <= 0.0f)
	{
		return;
	}

	// -----------------------------------------------------------------------
	// Update movement state
	// -----------------------------------------------------------------------
	Speed = OwningCharacter->GetVelocity().Size();
	bIsFalling = OwningCharacter->GetCharacterMovement()->IsFalling();

	// Sync committed state from character via Day 4 getter
	if (const auto* TitanChar = Cast<class ATitanCharacter>(OwningCharacter))
	{
		bIsCommitted = TitanChar->GetIsCommitted();
	}

	// -----------------------------------------------------------------------
	// Foot IK — only on the ground
	// -----------------------------------------------------------------------
	if (!bIsFalling)
	{
		// Step 1: Perform world line traces
		FFootIKTraceResult LeftTrace;
		FFootIKTraceResult RightTrace;
		PerformFootTraces(LeftTrace, RightTrace);

		// Step 2: Compute IK via pure math solver
		FFootIKResult LeftResult;
		FFootIKResult RightResult;
		float RawHipOffset = 0.0f;
		CalculateFootIKFromTraceResults(LeftTrace, RightTrace, LeftResult, RightResult, RawHipOffset);

		// Step 3: Interpolate for smooth transitions (no popping)
		const float InterpAlpha = FMath::Clamp(IKInterpSpeed * DeltaSeconds, 0.0f, 1.0f);

		LeftFootIKOffset = FMath::VInterpTo(PrevLeftIKOffset, LeftResult.IKOffset, DeltaSeconds, IKInterpSpeed);
		RightFootIKOffset = FMath::VInterpTo(PrevRightIKOffset, RightResult.IKOffset, DeltaSeconds, IKInterpSpeed);
		HipOffset = FMath::FInterpTo(PrevHipOffset, RawHipOffset, DeltaSeconds, IKInterpSpeed);
		LeftFootRotation = LeftResult.FootRotation;
		RightFootRotation = RightResult.FootRotation;

		// Cache for next frame
		PrevLeftIKOffset = LeftFootIKOffset;
		PrevRightIKOffset = RightFootIKOffset;
		PrevHipOffset = HipOffset;
	}
	else
	{
		// In the air — smoothly return to zero
		LeftFootIKOffset = FMath::VInterpTo(PrevLeftIKOffset, FVector::ZeroVector, DeltaSeconds, IKInterpSpeed);
		RightFootIKOffset = FMath::VInterpTo(PrevRightIKOffset, FVector::ZeroVector, DeltaSeconds, IKInterpSpeed);
		HipOffset = FMath::FInterpTo(PrevHipOffset, 0.0f, DeltaSeconds, IKInterpSpeed);
		LeftFootRotation = FRotator::ZeroRotator;
		RightFootRotation = FRotator::ZeroRotator;

		PrevLeftIKOffset = LeftFootIKOffset;
		PrevRightIKOffset = RightFootIKOffset;
		PrevHipOffset = HipOffset;
	}
}

// ============================================================================
// Pure Math IK Solver (STATIC — fully unit-testable)
// ============================================================================

void UTitanAnimInstance::CalculateFootIKFromTraceResults(
	const FFootIKTraceResult& LeftTrace,
	const FFootIKTraceResult& RightTrace,
	FFootIKResult& OutLeft,
	FFootIKResult& OutRight,
	float& OutHipOffset)
{
	// -----------------------------------------------------------------------
	// Step 1: Calculate raw Z offset for each foot
	// -----------------------------------------------------------------------
	// The Z offset represents how far the ground surface deviates from the
	// foot bone's current position. Positive = ground is above foot (push up).
	// Negative = ground is below foot (foot is floating).

	float LeftZDelta = 0.0f;
	float RightZDelta = 0.0f;

	if (LeftTrace.bHitGround)
	{
		// Delta = ground Z - foot Z (positive means ground is higher)
		LeftZDelta = LeftTrace.ImpactLocation.Z - LeftTrace.FootBoneLocation.Z;
	}

	if (RightTrace.bHitGround)
	{
		RightZDelta = RightTrace.ImpactLocation.Z - RightTrace.FootBoneLocation.Z;
	}

	// -----------------------------------------------------------------------
	// Step 2: Calculate hip offset
	// -----------------------------------------------------------------------
	// The hips must drop by the amount of the LOWEST foot delta.
	// This ensures the lower foot can physically reach its ground contact.
	//
	// Example:
	//   Left foot: ground is +30 above foot bone → LeftZDelta = +30
	//   Right foot: ground is -20 below foot bone → RightZDelta = -20
	//   → HipOffset = min(+30, -20) = -20 (hips drop 20 units)
	//   → Left IK offset = +30 - (-20) = +50 (left foot pushes up 50)
	//   → Right IK offset = -20 - (-20) = 0 (right foot stays at lowered hip)
	//
	// On flat ground: both deltas ≈ 0, hip offset ≈ 0

	OutHipOffset = FMath::Min(LeftZDelta, RightZDelta);

	// Clamp hip offset to prevent the character from sinking into the ground
	// beyond reason (e.g., if one foot is way off a cliff edge)
	OutHipOffset = FMath::Clamp(OutHipOffset, -50.0f, 0.0f);

	// -----------------------------------------------------------------------
	// Step 3: Calculate per-foot IK offsets (relative to lowered hip)
	// -----------------------------------------------------------------------
	// After hips lower, each foot's IK offset compensates for the difference
	// between its ground delta and the hip offset.

	if (LeftTrace.bHitGround)
	{
		OutLeft.IKOffset = FVector(0.0f, 0.0f, LeftZDelta - OutHipOffset);
	}
	else
	{
		// No ground contact — foot hangs naturally (no IK correction)
		OutLeft.IKOffset = FVector::ZeroVector;
	}

	if (RightTrace.bHitGround)
	{
		OutRight.IKOffset = FVector(0.0f, 0.0f, RightZDelta - OutHipOffset);
	}
	else
	{
		OutRight.IKOffset = FVector::ZeroVector;
	}

	// -----------------------------------------------------------------------
	// Step 4: Calculate foot rotations from surface normals
	// -----------------------------------------------------------------------

	if (LeftTrace.bHitGround)
	{
		OutLeft.FootRotation = CalculateFootRotationFromNormal(LeftTrace.ImpactNormal);
	}
	else
	{
		OutLeft.FootRotation = FRotator::ZeroRotator;
	}

	if (RightTrace.bHitGround)
	{
		OutRight.FootRotation = CalculateFootRotationFromNormal(RightTrace.ImpactNormal);
	}
	else
	{
		OutRight.FootRotation = FRotator::ZeroRotator;
	}
}

// ============================================================================
// Foot Rotation from Surface Normal (STATIC — fully unit-testable)
// ============================================================================

FRotator UTitanAnimInstance::CalculateFootRotationFromNormal(const FVector& ImpactNormal)
{
	// -----------------------------------------------------------------------
	// Convert a surface normal into a foot rotation (pitch + roll).
	//
	// Given a surface normal N = (Nx, Ny, Nz):
	//   - Pitch (Y rotation) = how much the surface tilts forward/backward
	//     → atan2(Nx, Nz) projected onto the foot's forward axis
	//   - Roll (X rotation) = how much the surface tilts left/right
	//     → atan2(Ny, Nz) projected onto the foot's right axis
	//
	// On flat ground, N = (0, 0, 1) → Pitch=0, Roll=0
	// On a forward slope, N = (0.5, 0, 0.866) → Pitch≈30°
	// -----------------------------------------------------------------------

	// Guard against degenerate normals (pointing straight sideways or down)
	if (ImpactNormal.Z < KINDA_SMALL_NUMBER)
	{
		return FRotator::ZeroRotator;
	}

	// Pitch: tilt around Y axis (forward/backward slope)
	// Negative because UE4 pitch convention: positive pitch = nose UP
	const float PitchDeg = FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.X, ImpactNormal.Z));

	// Roll: tilt around X axis (left/right slope)
	const float RollDeg = FMath::RadiansToDegrees(FMath::Atan2(ImpactNormal.Y, ImpactNormal.Z));

	return FRotator(PitchDeg, 0.0f, RollDeg);
}

// ============================================================================
// World-Dependent Foot Traces (NOT unit-testable — requires UWorld)
// ============================================================================

void UTitanAnimInstance::PerformFootTraces(
	FFootIKTraceResult& OutLeftTrace,
	FFootIKTraceResult& OutRightTrace)
{
	if (!OwningCharacter)
	{
		return;
	}

	const USkeletalMeshComponent* Mesh = OwningCharacter->GetMesh();
	if (!Mesh)
	{
		return;
	}

	UWorld* World = OwningCharacter->GetWorld();
	if (!World)
	{
		return;
	}

	// -----------------------------------------------------------------------
	// Collision query setup
	// -----------------------------------------------------------------------
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwningCharacter);
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = false;

	// Trace channel: Visibility for simplicity. Could use a custom channel later.
	const ECollisionChannel TraceChannel = ECC_Visibility;

	// Capsule half-height for reference offsets
	const float CapsuleHalfHeight = OwningCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	// -----------------------------------------------------------------------
	// Left foot trace
	// -----------------------------------------------------------------------
	{
		const FVector FootLoc = Mesh->GetSocketLocation(LeftFootBoneName);
		const FVector TraceStart = FVector(FootLoc.X, FootLoc.Y, OwningCharacter->GetActorLocation().Z);
		const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, CapsuleHalfHeight + TraceDistance);

		OutLeftTrace.FootBoneLocation = FootLoc;
		OutLeftTrace.FootBaseOffset = OwningCharacter->GetActorLocation().Z - CapsuleHalfHeight;

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, QueryParams))
		{
			OutLeftTrace.bHitGround = true;
			OutLeftTrace.ImpactLocation = Hit.ImpactPoint;
			OutLeftTrace.ImpactNormal = Hit.ImpactNormal;
		}
		else
		{
			OutLeftTrace.bHitGround = false;
		}
	}

	// -----------------------------------------------------------------------
	// Right foot trace
	// -----------------------------------------------------------------------
	{
		const FVector FootLoc = Mesh->GetSocketLocation(RightFootBoneName);
		const FVector TraceStart = FVector(FootLoc.X, FootLoc.Y, OwningCharacter->GetActorLocation().Z);
		const FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, CapsuleHalfHeight + TraceDistance);

		OutRightTrace.FootBoneLocation = FootLoc;
		OutRightTrace.FootBaseOffset = OwningCharacter->GetActorLocation().Z - CapsuleHalfHeight;

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, TraceChannel, QueryParams))
		{
			OutRightTrace.bHitGround = true;
			OutRightTrace.ImpactLocation = Hit.ImpactPoint;
			OutRightTrace.ImpactNormal = Hit.ImpactNormal;
		}
		else
		{
			OutRightTrace.bHitGround = false;
		}
	}
}
