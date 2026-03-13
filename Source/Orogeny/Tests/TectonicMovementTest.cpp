// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Tectonic Movement Physics Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// These tests verify the tectonic movement component's physics tuning
// via pure math. We prove the acceleration, braking, rotation, and
// stopping curves match our design constraints WITHOUT needing a live
// physics simulation.
//
// DESIGN TARGETS:
//   - MaxWalkSpeed: 150 cm/s (very slow, colossal feel)
//   - Time to max speed: ~0.6s theoretical, ~1.0-1.5s with friction
//   - Time to full stop: 3.0s theoretical, ~2.0s with friction
//   - Full 360° turn: 6.0 seconds (60°/s yaw)
//   - Mass: 100,000 kg (1000x default UE5 character)
//
// RUN: UnrealEditor-Cmd.exe ... -ExecCmds="Automation RunTests Orogeny.Movement"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "TectonicMovementComponent.h"

// ============================================================================
// Test 1: Component Default Values Match Specification
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTectonicDefaultsTest,
	"Orogeny.Movement.TectonicDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTectonicDefaultsTest::RunTest(const FString& Parameters)
{
	// Instantiate the component to verify constructor defaults
	UTectonicMovementComponent* MoveComp = NewObject<UTectonicMovementComponent>();

	TestEqual(TEXT("MaxWalkSpeed must be 150"),
		MoveComp->MaxWalkSpeed, 150.0f);

	TestEqual(TEXT("MaxAcceleration must be 250"),
		MoveComp->MaxAcceleration, 250.0f);

	TestEqual(TEXT("BrakingDecelerationWalking must be 50"),
		MoveComp->BrakingDecelerationWalking, 50.0f);

	TestEqual(TEXT("BrakingFrictionFactor must be 0.1"),
		MoveComp->BrakingFrictionFactor, 0.1f);

	TestTrue(TEXT("bUseSeparateBrakingFriction must be true"),
		MoveComp->bUseSeparateBrakingFriction);

	TestEqual(TEXT("Mass must be 100000"),
		MoveComp->Mass, 100000.0f);

	TestEqual(TEXT("RotationRate.Yaw must be 60"),
		MoveComp->RotationRate.Yaw, 60.0);

	TestTrue(TEXT("bOrientRotationToMovement must be true"),
		MoveComp->bOrientRotationToMovement);

	TestEqual(TEXT("GravityScale must be 1.5"),
		MoveComp->GravityScale, 1.5f);

	TestTrue(TEXT("AirControl must be very low (< 0.1)"),
		MoveComp->AirControl < 0.1f);

	return true;
}

// ============================================================================
// Test 2: Inertia Verification — Slow Acceleration
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTectonicInertiaTest,
	"Orogeny.Movement.InertiaVerification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTectonicInertiaTest::RunTest(const FString& Parameters)
{
	// After 0.5 seconds of full acceleration from rest:
	// v = MaxAcceleration * t = 250 * 0.5 = 125 cm/s
	// This is LESS than MaxWalkSpeed (150) — proving slow ramp-up
	const float VelocityAt05s = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterAcceleration(0.5f);

	TestTrue(TEXT("After 0.5s of accel, velocity (125) must be < MaxWalkSpeed (150)"),
		VelocityAt05s < UTectonicMovementComponent::TECTONIC_MAX_WALK_SPEED);

	TestEqual(TEXT("After 0.5s, theoretical velocity must be 125 cm/s"),
		VelocityAt05s, 125.0f);

	// After 0.2 seconds — barely moving:
	// v = 250 * 0.2 = 50 cm/s (only 33% of max speed)
	const float VelocityAt02s = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterAcceleration(0.2f);

	TestEqual(TEXT("After 0.2s, theoretical velocity must be 50 cm/s"),
		VelocityAt02s, 50.0f);

	TestTrue(TEXT("After 0.2s, velocity must be < 50% of MaxWalkSpeed"),
		VelocityAt02s < UTectonicMovementComponent::TECTONIC_MAX_WALK_SPEED * 0.5f);

	// After 1.0 seconds — should be capped at MaxWalkSpeed:
	// v = min(250 * 1.0, 150) = 150
	const float VelocityAt1s = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterAcceleration(1.0f);

	TestEqual(TEXT("After 1.0s, velocity must be capped at MaxWalkSpeed (150)"),
		VelocityAt1s, UTectonicMovementComponent::TECTONIC_MAX_WALK_SPEED);

	// Edge case: negative time
	const float VelocityNeg = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterAcceleration(-1.0f);
	TestEqual(TEXT("Negative time must yield 0 velocity"),
		VelocityNeg, 0.0f);

	return true;
}

// ============================================================================
// Test 3: Momentum / Braking Verification — No Instant Stop
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTectonicBrakingTest,
	"Orogeny.Movement.BrakingVerification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTectonicBrakingTest::RunTest(const FString& Parameters)
{
	const float MaxSpeed = UTectonicMovementComponent::TECTONIC_MAX_WALK_SPEED; // 150

	// After 1.0 second of coasting from MaxWalkSpeed:
	// v = 150 - 50 * 1.0 = 100 cm/s (still moving at 67% speed!)
	const float VelocityAfter1s = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterBraking(MaxSpeed, 1.0f);

	TestTrue(TEXT("After 1.0s of braking from max speed, velocity must be > 0"),
		VelocityAfter1s > 0.0f);

	TestEqual(TEXT("After 1.0s of braking, theoretical velocity must be 100 cm/s"),
		VelocityAfter1s, 100.0f);

	// After 2.0 seconds of coasting:
	// v = 150 - 50 * 2.0 = 50 cm/s (STILL moving at 33% speed!)
	const float VelocityAfter2s = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterBraking(MaxSpeed, 2.0f);

	TestTrue(TEXT("After 2.0s of braking, velocity must STILL be > 0"),
		VelocityAfter2s > 0.0f);

	TestEqual(TEXT("After 2.0s of braking, theoretical velocity must be 50 cm/s"),
		VelocityAfter2s, 50.0f);

	// After 3.0 seconds — theoretical full stop:
	// v = 150 - 50 * 3.0 = 0
	const float VelocityAfter3s = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterBraking(MaxSpeed, 3.0f);

	TestEqual(TEXT("After 3.0s of braking, velocity must reach 0"),
		VelocityAfter3s, 0.0f);

	// After 5.0 seconds — clamped at 0, never goes negative:
	const float VelocityAfter5s = UTectonicMovementComponent::CalculateTheoreticalVelocityAfterBraking(MaxSpeed, 5.0f);

	TestEqual(TEXT("Velocity must never go below 0"),
		VelocityAfter5s, 0.0f);

	// CRITICAL CONTRAST: Default UE5 character stops in ~0.1s.
	// Our Titan takes 3.0s (theoretical) / ~2.0s (with friction).
	// This is a 20-30x difference. The metaphor holds.

	return true;
}

// ============================================================================
// Test 4: Stopping Time Calculation
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTectonicStoppingTimeTest,
	"Orogeny.Movement.StoppingTime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTectonicStoppingTimeTest::RunTest(const FString& Parameters)
{
	const float MaxSpeed = UTectonicMovementComponent::TECTONIC_MAX_WALK_SPEED;

	// Time to stop from MaxWalkSpeed:
	// t = 150 / 50 = 3.0 seconds (pure deceleration, no friction)
	const float StopTime = UTectonicMovementComponent::CalculateMaxStoppingTime(MaxSpeed);

	TestEqual(TEXT("Max stopping time from max speed must be 3.0s"),
		StopTime, 3.0f);

	// This is the MAXIMUM — with BrakingFrictionFactor=0.1, actual stopping
	// will be slightly faster (~2.0-2.5s), but still massively longer than
	// the default UE5 character (~0.1s).

	// Half speed: 75 / 50 = 1.5s
	const float HalfStopTime = UTectonicMovementComponent::CalculateMaxStoppingTime(MaxSpeed * 0.5f);
	TestEqual(TEXT("Max stopping time from half speed must be 1.5s"),
		HalfStopTime, 1.5f);

	// Zero velocity: 0 / 50 = 0s
	const float ZeroStopTime = UTectonicMovementComponent::CalculateMaxStoppingTime(0.0f);
	TestEqual(TEXT("Stopping time from 0 must be 0"),
		ZeroStopTime, 0.0f);

	return true;
}

// ============================================================================
// Test 5: Rotation Rate — Slow Grind Turn
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTectonicRotationTest,
	"Orogeny.Movement.RotationRate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTectonicRotationTest::RunTest(const FString& Parameters)
{
	// At 60°/s, a full 360° turn takes 6.0 seconds
	const float FullTurnTime = 360.0f / UTectonicMovementComponent::TECTONIC_ROTATION_RATE_YAW;
	TestEqual(TEXT("Full 360 turn must take 6.0 seconds"),
		FullTurnTime, 6.0f);

	// After 1.0 second: max 60° of rotation
	const float RotationAt1s = UTectonicMovementComponent::CalculateMaxRotation(1.0f);
	TestEqual(TEXT("After 1.0s, max rotation must be 60 degrees"),
		RotationAt1s, 60.0f);

	// After 0.5 seconds: max 30° of rotation
	const float RotationAt05s = UTectonicMovementComponent::CalculateMaxRotation(0.5f);
	TestEqual(TEXT("After 0.5s, max rotation must be 30 degrees"),
		RotationAt05s, 30.0f);

	// CONTRAST: Default UE5 character at 540°/s would rotate 540° in 1 second.
	// Our Titan rotates 60°. That's 9x slower. The player must PLAN.
	TestTrue(TEXT("Tectonic rotation (60/s) must be < 15% of default UE5 (540/s)"),
		UTectonicMovementComponent::TECTONIC_ROTATION_RATE_YAW < 540.0f * 0.15f);

	return true;
}

// ============================================================================
// Test 6: Component Class Validation
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTectonicComponentClassTest,
	"Orogeny.Movement.ComponentClass",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTectonicComponentClassTest::RunTest(const FString& Parameters)
{
	// Verify that UTectonicMovementComponent IS a UCharacterMovementComponent
	// (required for ACharacter's movement pipeline)
	UTectonicMovementComponent* MoveComp = NewObject<UTectonicMovementComponent>();

	TestTrue(TEXT("UTectonicMovementComponent must be a UCharacterMovementComponent"),
		MoveComp->IsA<UCharacterMovementComponent>());

	TestTrue(TEXT("UTectonicMovementComponent must be a UMovementComponent"),
		MoveComp->IsA<UMovementComponent>());

	// Verify the constexpr constants are consistent
	TestEqual(TEXT("TECTONIC_MAX_WALK_SPEED constant must match constructed value"),
		MoveComp->MaxWalkSpeed, UTectonicMovementComponent::TECTONIC_MAX_WALK_SPEED);

	TestEqual(TEXT("TECTONIC_BRAKING_DECELERATION constant must match constructed value"),
		MoveComp->BrakingDecelerationWalking, UTectonicMovementComponent::TECTONIC_BRAKING_DECELERATION);

	return true;
}

// ============================================================================
// Test 7: Mass Comparison — Titan vs Default Character
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTectonicMassTest,
	"Orogeny.Movement.MassScale",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTectonicMassTest::RunTest(const FString& Parameters)
{
	UTectonicMovementComponent* MoveComp = NewObject<UTectonicMovementComponent>();

	// Default UE5 character mass is 100 kg
	const float DefaultUE5Mass = 100.0f;

	TestEqual(TEXT("Titan mass must be 100,000 kg"),
		MoveComp->Mass, 100000.0f);

	// Titan must be at least 100x heavier than a default character
	TestTrue(TEXT("Titan must be >= 100x heavier than default character"),
		MoveComp->Mass >= DefaultUE5Mass * 100.0f);

	// Verify this is exactly 1000x the default (our spec)
	TestEqual(TEXT("Titan must be exactly 1000x default mass"),
		MoveComp->Mass / DefaultUE5Mass, 1000.0f);

	return true;
}
