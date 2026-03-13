// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Difficulty Tuning Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify blight probability scaling with clamping.
// 2. Tests verify healing rate scaling.
// 3. Tests verify endless mode detection.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Tuning"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "TuningMath.h"

// ============================================================================
// Test 1: Blight Scaling - Hard Mode (0.20 × 2.0 = 0.40)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTuningBlightHardTest,
	"Orogeny.Tuning.Blight.HardMode",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTuningBlightHardTest::RunTest(const FString& Parameters)
{
	const float Scaled = FTuningMath::CalculateScaledBlightProbability(0.20f, 2.0f);
	TestEqual(TEXT("Hard mode: 0.20 × 2.0 = 0.40"), Scaled, 0.40f);
	return true;
}

// ============================================================================
// Test 2: Blight Scaling - Clamp (0.80 × 2.0 = 1.60 → clamped 1.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTuningBlightClampTest,
	"Orogeny.Tuning.Blight.Clamp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTuningBlightClampTest::RunTest(const FString& Parameters)
{
	const float Scaled = FTuningMath::CalculateScaledBlightProbability(0.80f, 2.0f);
	TestEqual(TEXT("Clamp: 0.80 × 2.0 → 1.0"), Scaled, 1.0f);
	return true;
}

// ============================================================================
// Test 3: Healing Scaling - Easy Mode (10.0 × 1.5 = 15.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTuningHealEasyTest,
	"Orogeny.Tuning.Healing.EasyMode",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTuningHealEasyTest::RunTest(const FString& Parameters)
{
	const float Scaled = FTuningMath::CalculateScaledEcosystemHealing(10.0f, 1.5f);
	TestEqual(TEXT("Easy mode: 10.0 × 1.5 = 15.0"), Scaled, 15.0f);
	return true;
}

// ============================================================================
// Test 4: Endless Mode - Standard (10.0 → false)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTuningEndlessStandardTest,
	"Orogeny.Tuning.Endless.Standard",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTuningEndlessStandardTest::RunTest(const FString& Parameters)
{
	const bool bEndless = FTuningMath::IsEndlessMode(10.0f);
	TestFalse(TEXT("Target=10 → not endless"), bEndless);
	return true;
}

// ============================================================================
// Test 5: Endless Mode - Active (0.0 → true)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTuningEndlessActiveTest,
	"Orogeny.Tuning.Endless.Active",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTuningEndlessActiveTest::RunTest(const FString& Parameters)
{
	const bool bEndless = FTuningMath::IsEndlessMode(0.0f);
	TestTrue(TEXT("Target=0 → endless mode"), bEndless);
	return true;
}
