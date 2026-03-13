// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Ecosystem Armor Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify slope angle validation using dot product math.
// 2. Tests verify altitude range enforcement.
// 3. Tests verify health normalization.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Ecosystem"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "EcosystemArmorComponent.h"

// ============================================================================
// Test 1: Validation.PerfectFlat — flat ground, valid altitude → true
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcoValidFlatTest,
	"Orogeny.Ecosystem.Validation.PerfectFlat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FEcoValidFlatTest::RunTest(const FString& Parameters)
{
	// Normal pointing straight up, altitude 5000 (within 1000-25000)
	const bool Result = UEcosystemArmorComponent::IsValidGrowthLocation(
		FVector(0.0f, 0.0f, 1.0f), 5000.0f, 45.0f, 1000.0f, 25000.0f);
	TestTrue(TEXT("Flat ground at valid altitude must allow growth"), Result);
	return true;
}

// ============================================================================
// Test 2: Validation.SteepCliff — 90° slope → false
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcoValidCliffTest,
	"Orogeny.Ecosystem.Validation.SteepCliff",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FEcoValidCliffTest::RunTest(const FString& Parameters)
{
	// Normal pointing sideways = 90° slope (pure cliff)
	const bool Result = UEcosystemArmorComponent::IsValidGrowthLocation(
		FVector(1.0f, 0.0f, 0.0f), 5000.0f, 45.0f, 1000.0f, 25000.0f);
	TestFalse(TEXT("90° cliff must reject growth"), Result);
	return true;
}

// ============================================================================
// Test 3: Validation.AltitudeTooLow — below minimum → false
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcoValidLowAltTest,
	"Orogeny.Ecosystem.Validation.AltitudeTooLow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FEcoValidLowAltTest::RunTest(const FString& Parameters)
{
	// Perfect flat but altitude 500 < MinGrowthAltitude 1000
	const bool Result = UEcosystemArmorComponent::IsValidGrowthLocation(
		FVector(0.0f, 0.0f, 1.0f), 500.0f, 45.0f, 1000.0f, 25000.0f);
	TestFalse(TEXT("Altitude below minimum must reject growth"), Result);
	return true;
}

// ============================================================================
// Test 4: Validation.AltitudeTooHigh — above max (snow line) → false
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcoValidHighAltTest,
	"Orogeny.Ecosystem.Validation.AltitudeTooHigh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FEcoValidHighAltTest::RunTest(const FString& Parameters)
{
	// Perfect flat but altitude 30000 > MaxGrowthAltitude 25000
	const bool Result = UEcosystemArmorComponent::IsValidGrowthLocation(
		FVector(0.0f, 0.0f, 1.0f), 30000.0f, 45.0f, 1000.0f, 25000.0f);
	TestFalse(TEXT("Altitude above snow line must reject growth"), Result);
	return true;
}

// ============================================================================
// Test 5: Health.Normalization — 25000/50000 → 0.5
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEcoHealthNormTest,
	"Orogeny.Ecosystem.Health.Normalization",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FEcoHealthNormTest::RunTest(const FString& Parameters)
{
	const float Health = UEcosystemArmorComponent::CalculateHealth(25000, 50000);
	TestEqual(TEXT("25000/50000 must equal 0.5 health"), Health, 0.5f);
	return true;
}
