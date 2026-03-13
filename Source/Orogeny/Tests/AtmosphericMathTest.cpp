// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Atmospheric Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify triangle-wave time-of-day mapping.
// 2. Tests verify blight-scaled fog density.
// 3. Tests verify scattering color blending.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Atmosphere.Math"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "AtmosphericMath.h"

// ============================================================================
// Test 1: Time of Day - Midnight (0° and 360° → 0.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphericMidnightTest,
	"Orogeny.Atmosphere.Math.TimeOfDay.Midnight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphericMidnightTest::RunTest(const FString& Parameters)
{
	const float Alpha0 = FAtmosphericMath::EvaluateTimeOfDayAlpha(0.0f);
	const float Alpha360 = FAtmosphericMath::EvaluateTimeOfDayAlpha(360.0f);

	TestEqual(TEXT("0° (Midnight) = 0.0"), Alpha0, 0.0f);
	TestEqual(TEXT("360° (Midnight) = 0.0"), Alpha360, 0.0f);
	return true;
}

// ============================================================================
// Test 2: Time of Day - Noon (180° → 1.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphericNoonTest,
	"Orogeny.Atmosphere.Math.TimeOfDay.Noon",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphericNoonTest::RunTest(const FString& Parameters)
{
	const float Alpha = FAtmosphericMath::EvaluateTimeOfDayAlpha(180.0f);
	TestEqual(TEXT("180° (Noon) = 1.0"), Alpha, 1.0f);
	return true;
}

// ============================================================================
// Test 3: Time of Day - Dawn/Dusk (90° → 0.5)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphericDawnDuskTest,
	"Orogeny.Atmosphere.Math.TimeOfDay.DawnDusk",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphericDawnDuskTest::RunTest(const FString& Parameters)
{
	const float Alpha = FAtmosphericMath::EvaluateTimeOfDayAlpha(90.0f);
	TestEqual(TEXT("90° (Dawn) = 0.5"), Alpha, 0.5f);
	return true;
}

// ============================================================================
// Test 4: Fog Density - Pure Nature (Blight = 0 → base density)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphericFogPureTest,
	"Orogeny.Atmosphere.Math.FogDensity.PureNature",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphericFogPureTest::RunTest(const FString& Parameters)
{
	const float Density = FAtmosphericMath::CalculateFogDensity(0.02f, 0.0f, 5.0f);
	TestEqual(TEXT("Zero blight = base fog density"), Density, 0.02f);
	return true;
}

// ============================================================================
// Test 5: Fog Density - Max Blight (Blight = 1 → base × 5)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphericFogMaxBlightTest,
	"Orogeny.Atmosphere.Math.FogDensity.MaxBlight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphericFogMaxBlightTest::RunTest(const FString& Parameters)
{
	const float Density = FAtmosphericMath::CalculateFogDensity(0.02f, 1.0f, 5.0f);
	TestEqual(TEXT("Full blight = base × 5 = 0.10"), Density, 0.10f);
	return true;
}

// ============================================================================
// Test 6: Color Blending (0.5 → exact midpoint)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphericColorBlendTest,
	"Orogeny.Atmosphere.Math.ColorBlending",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphericColorBlendTest::RunTest(const FString& Parameters)
{
	const FLinearColor Black(0.0f, 0.0f, 0.0f);
	const FLinearColor White(1.0f, 1.0f, 1.0f);

	const FLinearColor Result = FAtmosphericMath::BlendScatteringColor(
		Black, White, 0.5f);

	TestEqual(TEXT("R at 50% blend = 0.5"), Result.R, 0.5f);
	TestEqual(TEXT("G at 50% blend = 0.5"), Result.G, 0.5f);
	TestEqual(TEXT("B at 50% blend = 0.5"), Result.B, 0.5f);
	return true;
}
