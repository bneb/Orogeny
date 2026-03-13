// Copyright Orogeny. All Rights Reserved.

#include "TectonicMovementComponent.h"
#include "Orogeny.h"
#include "GameFramework/Character.h"

// ============================================================================
// Construction — Lock in tectonic movement feel
// ============================================================================

UTectonicMovementComponent::UTectonicMovementComponent()
{
	// -----------------------------------------------------------------------
	// Speed & Acceleration
	// -----------------------------------------------------------------------
	// MaxWalkSpeed is deliberately very slow. The Titan is colossal.
	// MaxAcceleration controls the ramp-up time: 150/250 = 0.6s theoretical,
	// but ground friction extends this to ~1.0-1.5s in practice.
	MaxWalkSpeed = TECTONIC_MAX_WALK_SPEED;          // 150 cm/s
	MaxAcceleration = TECTONIC_MAX_ACCELERATION;      // 250 cm/s²

	// -----------------------------------------------------------------------
	// Braking & Friction
	// -----------------------------------------------------------------------
	// The Titan does NOT stop on a dime. When input is released:
	//   BrakingDecelerationWalking = 50 → pure deceleration of 50 cm/s²
	//   BrakingFrictionFactor = 0.1 → minimal friction assist
	//   bUseSeparateBrakingFriction = true → decouple move friction from brake
	//
	// From MaxWalkSpeed (150): stopping time = 150/50 = 3.0s (theoretical max)
	// With friction: actual ~2.0s. Still massive compared to default UE5 (~0.1s).
	BrakingDecelerationWalking = TECTONIC_BRAKING_DECELERATION;  // 50 cm/s²
	BrakingFrictionFactor = TECTONIC_BRAKING_FRICTION;           // 0.1
	bUseSeparateBrakingFriction = true;

	// -----------------------------------------------------------------------
	// Mass & Physics
	// -----------------------------------------------------------------------
	// 100,000 kg. For reference, default UE5 character = 100 kg.
	// This is 1000x heavier. Physics pushes (Day 9 radial impulse)
	// will feel appropriately tectonic.
	Mass = TECTONIC_MASS;                             // 100,000 kg

	// -----------------------------------------------------------------------
	// Rotation — The "Grind" Turn
	// -----------------------------------------------------------------------
	// 60°/s yaw = 6 seconds to complete a full 360° turn.
	// Default UE5 = 540°/s. We are ~9x slower.
	// This forces the player to plan their direction of travel.
	bOrientRotationToMovement = true;
	RotationRate = FRotator(0.0f, TECTONIC_ROTATION_RATE_YAW, 0.0f);

	// -----------------------------------------------------------------------
	// Gravity
	// -----------------------------------------------------------------------
	// 1.5x gravity = the Titan slams down from jumps/falls with authority.
	// Also prevents the floaty feeling that kills the scale metaphor.
	GravityScale = TECTONIC_GRAVITY_SCALE;            // 1.5

	// -----------------------------------------------------------------------
	// Additional Tuning
	// -----------------------------------------------------------------------
	// Disable air control — when this thing is falling, it FALLS.
	// No mid-air direction changes for a mountain.
	AirControl = 0.05f;
	FallingLateralFriction = 0.0f;

	// Ground friction during movement (not braking) — moderate for responsiveness
	GroundFriction = 4.0f;

	// Prevent the "ice skating" feel at very low speeds
	bCanWalkOffLedges = true;
	bCanWalkOffLedgesWhenCrouching = false;

	UE_LOG(LogOrogeny, Log, TEXT("UTectonicMovementComponent initialized: MaxSpeed=%.0f, MaxAccel=%.0f, "
		"BrakingDecel=%.0f, BrakingFriction=%.2f, Mass=%.0f, RotationRate.Yaw=%.0f"),
		MaxWalkSpeed, MaxAcceleration, BrakingDecelerationWalking,
		BrakingFrictionFactor, Mass, RotationRate.Yaw);
}

// ============================================================================
// Pure Math Functions (STATIC — fully unit-testable)
// ============================================================================

float UTectonicMovementComponent::CalculateTheoreticalVelocityAfterAcceleration(float Seconds)
{
	// v(t) = min(a * t, MaxSpeed)
	// This is the theoretical maximum — real UE5 physics will be lower due to friction
	const float RawVelocity = TECTONIC_MAX_ACCELERATION * FMath::Max(Seconds, 0.0f);
	return FMath::Min(RawVelocity, TECTONIC_MAX_WALK_SPEED);
}

float UTectonicMovementComponent::CalculateTheoreticalVelocityAfterBraking(
	float InitialVelocity, float Seconds)
{
	// v(t) = max(v0 - decel * t, 0)
	// This is the SLOWEST braking (pure deceleration, no friction assist)
	// Actual UE5 braking with friction will be faster (velocity drops quicker)
	const float RawVelocity = InitialVelocity - (TECTONIC_BRAKING_DECELERATION * FMath::Max(Seconds, 0.0f));
	return FMath::Max(RawVelocity, 0.0f);
}

float UTectonicMovementComponent::CalculateMaxStoppingTime(float InitialVelocity)
{
	// t = v0 / decel
	if (TECTONIC_BRAKING_DECELERATION <= 0.0f)
	{
		return MAX_FLT; // Never stops without deceleration
	}
	return FMath::Max(InitialVelocity, 0.0f) / TECTONIC_BRAKING_DECELERATION;
}

float UTectonicMovementComponent::CalculateMaxRotation(float Seconds)
{
	return TECTONIC_ROTATION_RATE_YAW * FMath::Max(Seconds, 0.0f);
}

// ============================================================================
// Physics Rotation Override — The "Grind" Turn
// ============================================================================

void UTectonicMovementComponent::PhysicsRotation(float DeltaTime)
{
	// We let the parent handle the actual rotation math — our RotationRate
	// is already clamped to 60°/s in the constructor. The parent's
	// PhysicsRotation() will use bOrientRotationToMovement + RotationRate
	// to smoothly grind toward the velocity direction.
	//
	// We override here as a hook point for future enhancements:
	// - Day 5: Camera lag intensifies during turns
	// - Day 7: Dust VFX emit more during rotation (ground displacement)
	// - Day 10: Rock-grinding sound cue intensity scales with turn rate

	Super::PhysicsRotation(DeltaTime);

	// FUTURE: Calculate actual rotation delta for VFX/audio integration
	// const FRotator CurrentRotation = GetOwner()->GetActorRotation();
	// float RotationDelta = ... (compare with previous frame)
}
