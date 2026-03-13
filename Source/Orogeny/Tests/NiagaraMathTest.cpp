// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Niagara VFX Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify velocity → dust spawn rate scaling.
// 2. Tests verify depth → debris burst count.
// 3. Tests verify blight → smoke opacity.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.VFX"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "NiagaraMath.h"

// ============================================================================
// Test 1: Dust Rate - Still (0.0 → 0.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraDustStillTest,
	"Orogeny.VFX.Niagara.DustRate.Still",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraDustStillTest::RunTest(const FString& Parameters)
{
	const float Rate = FNiagaraMath::CalculateDustSpawnRate(0.0f);
	TestEqual(TEXT("Stationary = 0 dust"), Rate, 0.0f);
	return true;
}

// ============================================================================
// Test 2: Dust Rate - Max Speed (1.0 → 500.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraDustMaxTest,
	"Orogeny.VFX.Niagara.DustRate.MaxSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraDustMaxTest::RunTest(const FString& Parameters)
{
	const float Rate = FNiagaraMath::CalculateDustSpawnRate(1.0f, 500.0f);
	TestEqual(TEXT("Max speed = 500 dust/s"), Rate, 500.0f);
	return true;
}

// ============================================================================
// Test 3: Debris Burst - Surface (depth = 0 → 0 particles)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraDebrisSurfaceTest,
	"Orogeny.VFX.Niagara.DebrisBurst.Surface",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraDebrisSurfaceTest::RunTest(const FString& Parameters)
{
	const int32 Count = FNiagaraMath::CalculateDebrisBurstCount(0.0f, -15000.0f, 1000);
	TestEqual(TEXT("Surface = 0 debris"), Count, 0);
	return true;
}

// ============================================================================
// Test 4: Debris Burst - Max Depth (-15000 → 1000 particles)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraDebrisMaxTest,
	"Orogeny.VFX.Niagara.DebrisBurst.MaxDepth",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraDebrisMaxTest::RunTest(const FString& Parameters)
{
	const int32 Count = FNiagaraMath::CalculateDebrisBurstCount(-15000.0f, -15000.0f, 1000);
	TestEqual(TEXT("Max depth = 1000 debris"), Count, 1000);
	return true;
}

// ============================================================================
// Test 5: Smoke Opacity - Half Blight (0.5 → 0.5)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FNiagaraSmokeHalfTest,
	"Orogeny.VFX.Niagara.SmokeOpacity.HalfBlight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FNiagaraSmokeHalfTest::RunTest(const FString& Parameters)
{
	const float Opacity = FNiagaraMath::CalculateSmokeOpacity(0.5f, 1.0f);
	TestEqual(TEXT("Half blight = 0.5 opacity"), Opacity, 0.5f);
	return true;
}
