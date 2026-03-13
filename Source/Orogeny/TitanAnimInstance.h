// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TitanAnimInstance.generated.h"

// ============================================================================
// FFootIKTraceResult
// ============================================================================
// Pure data container for foot trace results. This struct decouples the
// IK math from the UWorld line trace system, enabling headless unit testing.
// In production, PerformFootTraces() fills these from real line traces.
// In tests, we construct them directly with mock data.
// ============================================================================

USTRUCT(BlueprintType)
struct OROGENY_API FFootIKTraceResult
{
	GENERATED_BODY()

	/** Whether the trace hit anything (if false, foot is in the air) */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	bool bHitGround = false;

	/** World-space location of the foot bone BEFORE IK adjustment */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FVector FootBoneLocation = FVector::ZeroVector;

	/** World-space location where the trace hit the ground */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FVector ImpactLocation = FVector::ZeroVector;

	/** Surface normal at the hit point (up = (0,0,1) on flat ground) */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FVector ImpactNormal = FVector::UpVector;

	/** Vertical distance from the character's base (capsule bottom) to the foot */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	float FootBaseOffset = 0.0f;
};

// ============================================================================
// FFootIKResult
// ============================================================================
// Output of the pure-math IK solver. Consumed by Animation Blueprint.
// ============================================================================

USTRUCT(BlueprintType)
struct OROGENY_API FFootIKResult
{
	GENERATED_BODY()

	/** IK effector offset (added to foot bone location in component space) */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FVector IKOffset = FVector::ZeroVector;

	/** Foot rotation to align with surface normal */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FRotator FootRotation = FRotator::ZeroRotator;
};

// ============================================================================
// UTitanAnimInstance
// ============================================================================
// Custom AnimInstance for the Titan character. Handles:
// - Foot placement IK via line traces (Day 2)
// - Committed action states (Day 4)
// - Root motion parsing (Day 3)
//
// DESIGN: The IK math is split into a pure function (CalculateFootIKFromTraceResults)
// that takes FFootIKTraceResult inputs and produces FFootIKResult outputs.
// This enables full TDD coverage without requiring a running UWorld.
// ============================================================================

UCLASS()
class OROGENY_API UTitanAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTitanAnimInstance();

	// -----------------------------------------------------------------------
	// UAnimInstance Interface
	// -----------------------------------------------------------------------
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// -----------------------------------------------------------------------
	// IK Output Properties (consumed by Animation Blueprint)
	// -----------------------------------------------------------------------

	/** Left foot IK offset in component space */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FVector LeftFootIKOffset = FVector::ZeroVector;

	/** Right foot IK offset in component space */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FVector RightFootIKOffset = FVector::ZeroVector;

	/** Hip offset — hips lower to accommodate the lowest foot */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	float HipOffset = 0.0f;

	/** Left foot rotation to match ground surface */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FRotator LeftFootRotation = FRotator::ZeroRotator;

	/** Right foot rotation to match ground surface */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|IK")
	FRotator RightFootRotation = FRotator::ZeroRotator;

	// -----------------------------------------------------------------------
	// Configuration
	// -----------------------------------------------------------------------

	/** Bone name for the left foot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|IK|Config")
	FName LeftFootBoneName = FName(TEXT("foot_l"));

	/** Bone name for the right foot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|IK|Config")
	FName RightFootBoneName = FName(TEXT("foot_r"));

	/** How far below the foot to trace for ground (in cm) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|IK|Config")
	float TraceDistance = 150.0f;

	/** Interpolation speed for IK offsets (higher = snappier, lower = smoother) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|IK|Config")
	float IKInterpSpeed = 15.0f;

	/** Maximum allowed IK offset to prevent extreme leg stretching */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|IK|Config")
	float MaxIKOffset = 50.0f;

	// -----------------------------------------------------------------------
	// Movement State (Day 3/4 will expand)
	// -----------------------------------------------------------------------

	/** Current character velocity magnitude */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|Movement")
	float Speed = 0.0f;

	/** Is the character currently in the air? */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|Movement")
	bool bIsFalling = false;

	/** Is the character in an un-cancelable committed action? (Day 4) */
	UPROPERTY(BlueprintReadOnly, Category = "Orogeny|Animation")
	bool bIsCommitted = false;

	// -----------------------------------------------------------------------
	// Pure Math IK Functions (TESTABLE — no UWorld dependency)
	// -----------------------------------------------------------------------

	/**
	 * Core IK solver. Takes trace results and produces IK output.
	 * This function is STATIC and PURE — no side effects, no world access.
	 * TDD tests call this directly with mock FFootIKTraceResult data.
	 *
	 * @param LeftTrace   Trace result for the left foot
	 * @param RightTrace  Trace result for the right foot
	 * @param OutLeft     Computed left foot IK result
	 * @param OutRight    Computed right foot IK result
	 * @param OutHipOffset Computed hip Z offset (always <= 0)
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|IK")
	static void CalculateFootIKFromTraceResults(
		const FFootIKTraceResult& LeftTrace,
		const FFootIKTraceResult& RightTrace,
		FFootIKResult& OutLeft,
		FFootIKResult& OutRight,
		float& OutHipOffset
	);

	/**
	 * Calculates foot rotation from a surface normal.
	 * Pure math — projects the normal onto pitch/roll axes.
	 *
	 * @param ImpactNormal  The surface normal at the foot contact point
	 * @return Rotator with Pitch and Roll to align foot to surface
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|IK")
	static FRotator CalculateFootRotationFromNormal(const FVector& ImpactNormal);

	// -----------------------------------------------------------------------
	// Accessors (for TDD convenience)
	// -----------------------------------------------------------------------

	float GetHipOffset() const { return HipOffset; }
	FVector GetLeftFootIKOffset() const { return LeftFootIKOffset; }
	FVector GetRightFootIKOffset() const { return RightFootIKOffset; }
	FRotator GetLeftFootRotation() const { return LeftFootRotation; }
	FRotator GetRightFootRotation() const { return RightFootRotation; }

protected:
	// -----------------------------------------------------------------------
	// World-Dependent Functions (NOT unit-testable — require UWorld)
	// -----------------------------------------------------------------------

	/**
	 * Performs the actual line traces in the world.
	 * Populates FFootIKTraceResult structs from real physics hits.
	 * Only called during NativeUpdateAnimation with a valid world.
	 */
	void PerformFootTraces(
		FFootIKTraceResult& OutLeftTrace,
		FFootIKTraceResult& OutRightTrace
	);

	/** Cached reference to the owning character */
	UPROPERTY()
	TObjectPtr<class ACharacter> OwningCharacter = nullptr;

private:
	/** Previous frame IK targets for interpolation smoothing */
	FVector PrevLeftIKOffset = FVector::ZeroVector;
	FVector PrevRightIKOffset = FVector::ZeroVector;
	float PrevHipOffset = 0.0f;
};
