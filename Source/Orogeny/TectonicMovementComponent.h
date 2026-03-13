// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TectonicMovementComponent.generated.h"

/**
 * UTectonicMovementComponent
 *
 * Custom movement component that replaces the default UCharacterMovementComponent
 * to simulate tectonic mass. A mountain does not walk — it displaces earth.
 *
 * Design Constraints:
 * - HIGH INERTIA: MaxAcceleration=250 means the Titan takes significant time
 *   to reach MaxWalkSpeed=150. This is deliberate.
 * - HEAVY BRAKING: BrakingDecelerationWalking=50 with BrakingFrictionFactor=0.1
 *   means the Titan grinds to a halt over a long distance, never stopping on a dime.
 * - SLOW ROTATION: RotationRate.Yaw=60°/s forces the player to commit to a
 *   direction long before arriving. No instant snap turns.
 * - MASSIVE PHYSICS: Mass=100000 ensures physics interactions (Day 9 radial impulse)
 *   feel appropriately scaled.
 *
 * Mathematical Proof (verified by TDD):
 *   - Acceleration: After 0.5s of input, velocity < MaxWalkSpeed (150)
 *     Pure math: MaxAcceleration * 0.5s = 125 < 150 ✓
 *   - Braking: After 1.0s of coast from MaxWalkSpeed, velocity > 0
 *     Pure math: 150 - (BrakingDecel * 1.0s) = 150 - 50 = 100 > 0 ✓
 *   - Full stop: 150 / 50 = 3.0 seconds to halt (with friction, ~2.0s actual)
 */
UCLASS()
class OROGENY_API UTectonicMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UTectonicMovementComponent();

	// -----------------------------------------------------------------------
	// Tectonic Constants (exposed for TDD verification)
	// -----------------------------------------------------------------------

	/** Target max walk speed for the Titan (cm/s) */
	static constexpr float TECTONIC_MAX_WALK_SPEED = 150.0f;

	/** Maximum acceleration rate (cm/s²) */
	static constexpr float TECTONIC_MAX_ACCELERATION = 250.0f;

	/** Braking deceleration when walking (cm/s²) */
	static constexpr float TECTONIC_BRAKING_DECELERATION = 50.0f;

	/** Braking friction factor — extremely low for sliding feel */
	static constexpr float TECTONIC_BRAKING_FRICTION = 0.1f;

	/** Titan mass for physics interactions (kg) */
	static constexpr float TECTONIC_MASS = 100000.0f;

	/** Maximum yaw rotation rate (degrees/second) */
	static constexpr float TECTONIC_ROTATION_RATE_YAW = 60.0f;

	/** Gravity scale — slightly heavier than normal for grounded feel */
	static constexpr float TECTONIC_GRAVITY_SCALE = 1.5f;

	// -----------------------------------------------------------------------
	// Pure Math Functions (TESTABLE — no UWorld dependency)
	// -----------------------------------------------------------------------

	/**
	 * Calculates the theoretical maximum velocity after a given time
	 * of constant acceleration from rest, capped at MaxWalkSpeed.
	 *
	 * v(t) = min(MaxAcceleration * t, MaxWalkSpeed)
	 *
	 * NOTE: Real UE5 movement includes ground friction, so actual velocity
	 * will be LOWER than this theoretical max. This provides an upper bound.
	 *
	 * @param Seconds  Duration of constant acceleration input
	 * @return Upper bound of velocity after the given time
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Movement")
	static float CalculateTheoreticalVelocityAfterAcceleration(float Seconds);

	/**
	 * Calculates the theoretical velocity after braking from a given
	 * initial velocity for a given duration.
	 *
	 * v(t) = max(InitialVelocity - BrakingDeceleration * t, 0)
	 *
	 * NOTE: Real UE5 braking also applies ground friction (BrakingFrictionFactor),
	 * so actual velocity will be LOWER. This provides an upper bound for "still moving"
	 * tests and a proof that the character doesn't stop instantly.
	 *
	 * @param InitialVelocity  Starting velocity (cm/s)
	 * @param Seconds          Duration of braking with no input
	 * @return Lower bound of velocity (actual may differ due to friction)
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Movement")
	static float CalculateTheoreticalVelocityAfterBraking(float InitialVelocity, float Seconds);

	/**
	 * Calculates the time required to reach a full stop from a given velocity.
	 *
	 * t_stop = InitialVelocity / BrakingDeceleration
	 *
	 * This is the MAXIMUM stopping time (pure deceleration, no friction assist).
	 * With friction, actual stopping time will be shorter.
	 *
	 * @param InitialVelocity  Starting velocity (cm/s)
	 * @return Maximum time to stop (seconds)
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Movement")
	static float CalculateMaxStoppingTime(float InitialVelocity);

	/**
	 * Calculates the maximum rotation achievable in a given time.
	 *
	 * @param Seconds  Duration of rotation
	 * @return Maximum yaw rotation in degrees
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Movement")
	static float CalculateMaxRotation(float Seconds);

protected:
	// -----------------------------------------------------------------------
	// UCharacterMovementComponent Overrides
	// -----------------------------------------------------------------------

	/**
	 * Override to enforce tectonic rotation constraints.
	 * The Titan cannot snap-turn. Rotation is a slow grind.
	 */
	virtual void PhysicsRotation(float DeltaTime) override;
};
