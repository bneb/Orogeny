// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: UI Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify day → century conversion (1-indexed).
// 2. Tests verify health → integer percentage.
// 3. Tests verify blight ratio with divide-by-zero safety.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.UI"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "UIMath.h"

// ============================================================================
// Test 1: Century - Day 0 → Century 1
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUIMathCenturyDay0Test,
	"Orogeny.UI.Math.Century.Day0",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FUIMathCenturyDay0Test::RunTest(const FString& Parameters)
{
	const int32 Century = FUIMath::CalculateCurrentCentury(0.0f);
	TestEqual(TEXT("Day 0 = Century 1"), Century, 1);
	return true;
}

// ============================================================================
// Test 2: Century - Boundary (36524.9 → 1, 36525.0 → 2)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUIMathCenturyBoundaryTest,
	"Orogeny.UI.Math.Century.Boundary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FUIMathCenturyBoundaryTest::RunTest(const FString& Parameters)
{
	const int32 LastDayC1 = FUIMath::CalculateCurrentCentury(36524.9f);
	const int32 FirstDayC2 = FUIMath::CalculateCurrentCentury(36525.0f);

	TestEqual(TEXT("Day 36524.9 = still Century 1"), LastDayC1, 1);
	TestEqual(TEXT("Day 36525.0 = Century 2"), FirstDayC2, 2);
	return true;
}

// ============================================================================
// Test 3: Health Percentage (0.456 → 45%)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUIMathHealthPercentTest,
	"Orogeny.UI.Math.HealthPercentage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FUIMathHealthPercentTest::RunTest(const FString& Parameters)
{
	const int32 Percent = FUIMath::CalculateHealthPercentage(0.456f, 1.0f);
	TestEqual(TEXT("Health 0.456 = 45%"), Percent, 45);
	return true;
}

// ============================================================================
// Test 4: Blight Threat - Safe Division (total = 0 → no crash)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUIMathBlightSafeDivTest,
	"Orogeny.UI.Math.BlightThreat.SafeDiv",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FUIMathBlightSafeDivTest::RunTest(const FString& Parameters)
{
	const float Threat = FUIMath::CalculateBlightThreatLevel(10, 0);
	TestEqual(TEXT("Total=0 → safe return 0.0f"), Threat, 0.0f);
	return true;
}

// ============================================================================
// Test 5: Blight Threat - Ratio (25/100 → 0.25)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FUIMathBlightRatioTest,
	"Orogeny.UI.Math.BlightThreat.Ratio",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FUIMathBlightRatioTest::RunTest(const FString& Parameters)
{
	const float Threat = FUIMath::CalculateBlightThreatLevel(25, 100);
	TestEqual(TEXT("25/100 = 0.25 threat"), Threat, 0.25f);
	return true;
}
