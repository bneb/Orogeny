// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Subduction Mechanics Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify linear Z interpolation at constant speed.
// 2. Tests verify snap-to-target prevents Z jitter.
// 3. Tests verify state machine toggle.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Subduction"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "TitanSubductionComponent.h"

// ============================================================================
// Test 1: Interpolation.Sinking — 0 → -2000 in 1 second at 2000 cm/s
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSubductionSinkingTest,
	"Orogeny.Subduction.Interpolation.Sinking",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSubductionSinkingTest::RunTest(const FString& Parameters)
{
	// At surface (0), target deep (-15000), speed 2000, 1 second = move 2000 down
	const float Result = UTitanSubductionComponent::CalculateSubductionZ(
		0.0f, -15000.0f, 2000.0f, 1.0f);
	TestEqual(TEXT("1 second at 2000 cm/s from 0 → -2000"),
		Result, -2000.0f);
	return true;
}

// ============================================================================
// Test 2: Interpolation.Emerging — -15000 → -13000 in 1 second
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSubductionEmergingTest,
	"Orogeny.Subduction.Interpolation.Emerging",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSubductionEmergingTest::RunTest(const FString& Parameters)
{
	// At depth (-15000), target surface (0), speed 2000, 1 second = move 2000 up
	const float Result = UTitanSubductionComponent::CalculateSubductionZ(
		-15000.0f, 0.0f, 2000.0f, 1.0f);
	TestEqual(TEXT("1 second at 2000 cm/s from -15000 → -13000"),
		Result, -13000.0f);
	return true;
}

// ============================================================================
// Test 3: Interpolation.SnapToTarget — prevents jitter past floor
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSubductionSnapTest,
	"Orogeny.Subduction.Interpolation.SnapToTarget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSubductionSnapTest::RunTest(const FString& Parameters)
{
	// 500 units from target, but speed allows 2000 per second
	// FInterpConstantTo MUST snap to target, not overshoot
	const float Result = UTitanSubductionComponent::CalculateSubductionZ(
		-14500.0f, -15000.0f, 2000.0f, 1.0f);
	TestEqual(TEXT("500 from target at 2000 speed must snap to -15000 (no overshoot)"),
		Result, -15000.0f);
	return true;
}

// ============================================================================
// Test 4: StateMachine.Toggle — bIsTransitioning = true after toggle
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSubductionToggleTest,
	"Orogeny.Subduction.StateMachine.Toggle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSubductionToggleTest::RunTest(const FString& Parameters)
{
	// Construct a component directly (no owner needed for state test)
	UTitanSubductionComponent* Comp = NewObject<UTitanSubductionComponent>();

	// Initial state
	TestFalse(TEXT("Initial: bIsSubducted must be false"), Comp->bIsSubducted);
	TestFalse(TEXT("Initial: bIsTransitioning must be false"), Comp->bIsTransitioning);

	// Toggle — should begin sinking
	Comp->ToggleSubduction();
	TestTrue(TEXT("After toggle: bIsSubducted must be true"), Comp->bIsSubducted);
	TestTrue(TEXT("After toggle: bIsTransitioning must be true"), Comp->bIsTransitioning);

	return true;
}
