// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Biome Material Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify slope → rock weight mapping.
// 2. Tests verify altitude → snow weight mapping.
// 3. Tests verify rock override (snow can't accumulate on cliffs).
// 4. Tests verify weights sum to 1.0.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Biome"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "ProceduralTerrainMath.h"

// ============================================================================
// Test 1: Flat Valley — Grass = 1.0, Rock = 0.0, Snow = 0.0
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBiomeFlatValleyTest,
	"Orogeny.Biome.FlatValley",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FBiomeFlatValleyTest::RunTest(const FString& Parameters)
{
	// Low altitude (5000 cm = 50m), perfectly flat (normal = straight up)
	const FBiomeWeights W = FProceduralTerrainMath::CalculateBiomeWeights(
		5000.0f, FVector(0.0f, 0.0f, 1.0f));

	TestEqual(TEXT("Flat valley: GrassWeight = 1.0"), W.GrassWeight, 1.0f);
	TestEqual(TEXT("Flat valley: RockWeight = 0.0"), W.RockWeight, 0.0f);
	TestEqual(TEXT("Flat valley: SnowWeight = 0.0"), W.SnowWeight, 0.0f);
	return true;
}

// ============================================================================
// Test 2: Steep Cliff — Rock = 1.0, Grass = 0.0, Snow = 0.0
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBiomeSteepCliffTest,
	"Orogeny.Biome.SteepCliff",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FBiomeSteepCliffTest::RunTest(const FString& Parameters)
{
	// Low altitude, perfectly vertical (normal = horizontal)
	const FBiomeWeights W = FProceduralTerrainMath::CalculateBiomeWeights(
		5000.0f, FVector(1.0f, 0.0f, 0.0f));

	TestEqual(TEXT("Steep cliff: RockWeight = 1.0"), W.RockWeight, 1.0f);
	TestEqual(TEXT("Steep cliff: GrassWeight = 0.0"), W.GrassWeight, 0.0f);
	TestEqual(TEXT("Steep cliff: SnowWeight = 0.0"), W.SnowWeight, 0.0f);
	return true;
}

// ============================================================================
// Test 3: High Altitude Flat — Snow = 1.0, Grass = 0.0, Rock = 0.0
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBiomeHighAltitudeFlatTest,
	"Orogeny.Biome.HighAltitudeFlat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FBiomeHighAltitudeFlatTest::RunTest(const FString& Parameters)
{
	// High altitude (30000 cm = 300m), flat surface
	const FBiomeWeights W = FProceduralTerrainMath::CalculateBiomeWeights(
		30000.0f, FVector(0.0f, 0.0f, 1.0f));

	TestEqual(TEXT("High flat: SnowWeight = 1.0"), W.SnowWeight, 1.0f);
	TestEqual(TEXT("High flat: GrassWeight = 0.0"), W.GrassWeight, 0.0f);
	TestEqual(TEXT("High flat: RockWeight = 0.0"), W.RockWeight, 0.0f);
	return true;
}

// ============================================================================
// Test 4: High Altitude Cliff — Rock Override (Snow cannot sit on cliffs)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBiomeHighAltitudeCliffTest,
	"Orogeny.Biome.HighAltitudeCliff",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FBiomeHighAltitudeCliffTest::RunTest(const FString& Parameters)
{
	// High altitude AND vertical face — rock MUST override snow
	const FBiomeWeights W = FProceduralTerrainMath::CalculateBiomeWeights(
		30000.0f, FVector(1.0f, 0.0f, 0.0f));

	TestEqual(TEXT("High cliff: RockWeight = 1.0 (override)"), W.RockWeight, 1.0f);
	TestEqual(TEXT("High cliff: SnowWeight = 0.0 (can't sit on cliff)"), W.SnowWeight, 0.0f);
	TestEqual(TEXT("High cliff: GrassWeight = 0.0"), W.GrassWeight, 0.0f);
	return true;
}
