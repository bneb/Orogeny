// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Feedback Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify velocity → shake scale mapping with dead zone.
// 2. Tests verify health → vignette weight with critical threshold.
// 3. Tests verify subduction depth → rumble intensity.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Feedback"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "FeedbackMath.h"

// ============================================================================
// Test 1: Walk Shake - Standing Still (0.0 → 0.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFeedbackShakeStillTest,
	"Orogeny.Feedback.FootstepShake.Still",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFeedbackShakeStillTest::RunTest(const FString& Parameters)
{
	const float Scale = FFeedbackMath::CalculateFootstepShakeScale(0.0f);
	TestEqual(TEXT("Standing still = 0.0 shake"), Scale, 0.0f);
	return true;
}

// ============================================================================
// Test 2: Walk Shake - Max Speed (1.0 → 1.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFeedbackShakeMaxTest,
	"Orogeny.Feedback.FootstepShake.MaxSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFeedbackShakeMaxTest::RunTest(const FString& Parameters)
{
	const float Scale = FFeedbackMath::CalculateFootstepShakeScale(1.0f);
	TestEqual(TEXT("Max speed = 1.0 shake"), Scale, 1.0f);
	return true;
}

// ============================================================================
// Test 3: Health Vignette - Healthy (1.0 → 0.0, no vignette)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFeedbackVignetteHealthyTest,
	"Orogeny.Feedback.HealthVignette.Healthy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFeedbackVignetteHealthyTest::RunTest(const FString& Parameters)
{
	const float Weight = FFeedbackMath::CalculateHealthVignetteWeight(1.0f, 1.0f, 0.3f);
	TestEqual(TEXT("Healthy (1.0) = no vignette"), Weight, 0.0f);
	return true;
}

// ============================================================================
// Test 4: Health Vignette - Critical (0.15 → 0.5)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFeedbackVignetteCriticalTest,
	"Orogeny.Feedback.HealthVignette.Critical",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFeedbackVignetteCriticalTest::RunTest(const FString& Parameters)
{
	const float Weight = FFeedbackMath::CalculateHealthVignetteWeight(0.15f, 1.0f, 0.3f);
	TestEqual(TEXT("Critical (0.15) = 0.5 vignette"), Weight, 0.5f);
	return true;
}

// ============================================================================
// Test 5: Health Vignette - Dead (0.0 → 1.0, full darkness)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFeedbackVignetteDeadTest,
	"Orogeny.Feedback.HealthVignette.Dead",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFeedbackVignetteDeadTest::RunTest(const FString& Parameters)
{
	const float Weight = FFeedbackMath::CalculateHealthVignetteWeight(0.0f, 1.0f, 0.3f);
	TestEqual(TEXT("Dead (0.0) = 1.0 full vignette"), Weight, 1.0f);
	return true;
}

// ============================================================================
// Test 6: Subduction Rumble - Idle (not transitioning → 0.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFeedbackRumbleIdleTest,
	"Orogeny.Feedback.SubductionRumble.Idle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFeedbackRumbleIdleTest::RunTest(const FString& Parameters)
{
	const float Intensity = FFeedbackMath::CalculateSubductionRumbleIntensity(
		false, -5000.0f, -15000.0f);
	TestEqual(TEXT("Not transitioning = 0.0 rumble"), Intensity, 0.0f);
	return true;
}
