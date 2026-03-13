// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Combat (Orographic Lift) Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify Supercell's WeakenStorm math obeys floor clamps.
// 2. If speed can hit 0, the storm stops = broken. If opacity hits 0,
//    the storm vanishes = broken. TDD prevents both.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Combat"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "Supercell_Actor.h"
#include "OrographicLiftComponent.h"

// ============================================================================
// Test: Standard Weaken — Speed 50→35, Opacity 1.0→0.75
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCombatStandardWeakenTest,
	"Orogeny.Combat.Lift.StandardWeaken",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCombatStandardWeakenTest::RunTest(const FString& Parameters)
{
	ASupercell_Actor* Storm = NewObject<ASupercell_Actor>();

	// Pre-conditions
	TestEqual(TEXT("Pre: speed must be 50"), Storm->MovementSpeed, 50.0f);
	TestEqual(TEXT("Pre: opacity must be 1.0"), Storm->CloudOpacity, 1.0f);

	// Apply standard damage
	Storm->WeakenStorm(15.0f, 0.25f);

	TestEqual(TEXT("Post: speed must be 35"), Storm->MovementSpeed, 35.0f);
	TestEqual(TEXT("Post: opacity must be 0.75"), Storm->CloudOpacity, 0.75f);

	return true;
}

// ============================================================================
// Test: Floor Clamping (Speed) — Never drops below 10
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCombatSpeedFloorTest,
	"Orogeny.Combat.Lift.SpeedFloorClamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCombatSpeedFloorTest::RunTest(const FString& Parameters)
{
	ASupercell_Actor* Storm = NewObject<ASupercell_Actor>();

	// Apply massive speed reduction
	Storm->WeakenStorm(100.0f, 0.0f);

	TestEqual(TEXT("Speed must clamp to MIN (10)"),
		Storm->MovementSpeed, ASupercell_Actor::MIN_MOVEMENT_SPEED);
	TestEqual(TEXT("MIN_MOVEMENT_SPEED must be 10"),
		ASupercell_Actor::MIN_MOVEMENT_SPEED, 10.0f);

	return true;
}

// ============================================================================
// Test: Floor Clamping (Opacity) — Never drops below 0.2
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCombatOpacityFloorTest,
	"Orogeny.Combat.Lift.OpacityFloorClamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCombatOpacityFloorTest::RunTest(const FString& Parameters)
{
	ASupercell_Actor* Storm = NewObject<ASupercell_Actor>();

	// Apply massive opacity reduction
	Storm->WeakenStorm(0.0f, 2.0f);

	TestEqual(TEXT("Opacity must clamp to MIN (0.2)"),
		Storm->CloudOpacity, ASupercell_Actor::MIN_CLOUD_OPACITY);
	TestEqual(TEXT("MIN_CLOUD_OPACITY must be 0.2"),
		ASupercell_Actor::MIN_CLOUD_OPACITY, 0.2f);

	return true;
}

// ============================================================================
// Test: Multiple Weakens — Cumulative damage with floor
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCombatMultipleWeakensTest,
	"Orogeny.Combat.Lift.MultipleWeakens",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCombatMultipleWeakensTest::RunTest(const FString& Parameters)
{
	ASupercell_Actor* Storm = NewObject<ASupercell_Actor>();

	// Three hits of 15 speed / 0.25 opacity each
	// Speed: 50 → 35 → 20 → 10 (floors at 10, not 5)
	// Opacity: 1.0 → 0.75 → 0.50 → 0.25
	Storm->WeakenStorm(15.0f, 0.25f);
	Storm->WeakenStorm(15.0f, 0.25f);
	Storm->WeakenStorm(15.0f, 0.25f);

	TestEqual(TEXT("After 3 hits: speed must floor at 10"),
		Storm->MovementSpeed, 10.0f);
	TestEqual(TEXT("After 3 hits: opacity must be 0.25"),
		Storm->CloudOpacity, 0.25f);

	// One more hit
	Storm->WeakenStorm(15.0f, 0.25f);

	TestEqual(TEXT("After 4 hits: speed still floors at 10"),
		Storm->MovementSpeed, 10.0f);
	TestEqual(TEXT("After 4 hits: opacity must floor at 0.2"),
		Storm->CloudOpacity, 0.2f);

	return true;
}

// ============================================================================
// Test: Component Architecture — LiftRadius default
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCombatLiftArchTest,
	"Orogeny.Combat.Lift.ComponentArchitecture",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCombatLiftArchTest::RunTest(const FString& Parameters)
{
	UOrographicLiftComponent* Comp = NewObject<UOrographicLiftComponent>();

	TestEqual(TEXT("Default LiftRadius must be 30000"),
		Comp->LiftRadius, UOrographicLiftComponent::DEFAULT_LIFT_RADIUS);
	TestEqual(TEXT("DEFAULT_LIFT_RADIUS constant must be 30000"),
		UOrographicLiftComponent::DEFAULT_LIFT_RADIUS, 30000.0f);

	TestEqual(TEXT("Default SpeedDamage must be 15"),
		Comp->SpeedDamage, UOrographicLiftComponent::DEFAULT_SPEED_DAMAGE);
	TestEqual(TEXT("Default OpacityDamage must be 0.25"),
		Comp->OpacityDamage, UOrographicLiftComponent::DEFAULT_OPACITY_DAMAGE);

	return true;
}
