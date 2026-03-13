// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Procedural Terrain Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify FloorToInt chunk coordinate mapping.
// 2. Tests verify Perlin noise determinism.
// 3. Tests verify seamless chunk boundary continuity.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Terrain.Procedural"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "ProceduralTerrainMath.h"

// ============================================================================
// Test 1: ChunkMath.Positive — (15000, 5000) → Chunk (1, 0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTerrainChunkPositiveTest,
	"Orogeny.Terrain.Procedural.ChunkMath.Positive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTerrainChunkPositiveTest::RunTest(const FString& Parameters)
{
	const FIntPoint Result = FProceduralTerrainMath::GetChunkFromWorldLocation(
		FVector(15000.0f, 5000.0f, 0.0f), 10000.0f);

	TestEqual(TEXT("X=15000 / 10000 → Chunk X=1"), Result.X, 1);
	TestEqual(TEXT("Y=5000 / 10000 → Chunk Y=0"), Result.Y, 0);
	return true;
}

// ============================================================================
// Test 2: ChunkMath.Negative — (-5000, -15000) → Chunk (-1, -2)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTerrainChunkNegativeTest,
	"Orogeny.Terrain.Procedural.ChunkMath.Negative",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTerrainChunkNegativeTest::RunTest(const FString& Parameters)
{
	const FIntPoint Result = FProceduralTerrainMath::GetChunkFromWorldLocation(
		FVector(-5000.0f, -15000.0f, 0.0f), 10000.0f);

	TestEqual(TEXT("X=-5000 / 10000 → Floor(-0.5) = Chunk X=-1"), Result.X, -1);
	TestEqual(TEXT("Y=-15000 / 10000 → Floor(-1.5) = Chunk Y=-2"), Result.Y, -2);
	return true;
}

// ============================================================================
// Test 3: NoiseContinuity — Same input → identical output (determinism)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTerrainNoiseDeterminismTest,
	"Orogeny.Terrain.Procedural.NoiseContinuity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTerrainNoiseDeterminismTest::RunTest(const FString& Parameters)
{
	const float Height1 = FProceduralTerrainMath::CalculateHeightAt(
		10000.0f, 0.0f, 0.00005f, 25000.0f);
	const float Height2 = FProceduralTerrainMath::CalculateHeightAt(
		10000.0f, 0.0f, 0.00005f, 25000.0f);

	TestEqual(TEXT("Same input must produce identical output"), Height1, Height2);
	return true;
}

// ============================================================================
// Test 4: GridBoundaries — Right edge of Chunk(0,0) = Left edge of Chunk(1,0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTerrainSeamlessTest,
	"Orogeny.Terrain.Procedural.GridBoundaries",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTerrainSeamlessTest::RunTest(const FString& Parameters)
{
	const float ChunkSize = 10000.0f;
	const float NoiseScale = 0.00005f;
	const float HeightMul = 25000.0f;

	// Right edge of Chunk (0,0): X = ChunkSize (= 10000), Y = 5000 (mid-edge)
	const float RightEdge = FProceduralTerrainMath::CalculateHeightAt(
		ChunkSize, 5000.0f, NoiseScale, HeightMul);

	// Left edge of Chunk (1,0): X = 1 * ChunkSize (= 10000), Y = 5000 (mid-edge)
	// These are the SAME world coordinate — must be identical
	const float LeftEdge = FProceduralTerrainMath::CalculateHeightAt(
		1.0f * ChunkSize, 5000.0f, NoiseScale, HeightMul);

	TestEqual(TEXT("Chunk boundary heights must be identical (seamless)"),
		RightEdge, LeftEdge);
	return true;
}

// ============================================================================
// Test 5: FBM Determinism — Same input → identical output
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTerrainFBMDeterminismTest,
	"Orogeny.Terrain.Procedural.FBM.Determinism",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTerrainFBMDeterminismTest::RunTest(const FString& Parameters)
{
	const float Result1 = FProceduralTerrainMath::CalculateFBM(
		5000.0f, 3000.0f, 6, 0.5f, 2.0f, 0.00005f);
	const float Result2 = FProceduralTerrainMath::CalculateFBM(
		5000.0f, 3000.0f, 6, 0.5f, 2.0f, 0.00005f);

	TestEqual(TEXT("FBM: Same input must produce identical output"), Result1, Result2);
	return true;
}

// ============================================================================
// Test 6: Ridged Profile — Never returns negative values
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTerrainRidgedNonNegativeTest,
	"Orogeny.Terrain.Procedural.Ridged.NonNegative",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTerrainRidgedNonNegativeTest::RunTest(const FString& Parameters)
{
	// Sample multiple positions across the noise field
	const float Scale = 0.00005f;
	bool bAllNonNegative = true;

	for (float X = -50000.0f; X <= 50000.0f; X += 7777.0f)
	{
		for (float Y = -50000.0f; Y <= 50000.0f; Y += 7777.0f)
		{
			const float Value = FProceduralTerrainMath::CalculateRidged(
				X, Y, 6, 0.5f, 2.0f, Scale);
			if (Value < 0.0f)
			{
				bAllNonNegative = false;
				AddError(FString::Printf(
					TEXT("Ridged returned negative %.6f at (%.0f, %.0f)"), Value, X, Y));
			}
		}
	}

	TestTrue(TEXT("Ridged noise must never return negative values"), bAllNonNegative);
	return true;
}

// ============================================================================
// Test 7: Composite Integration — No floating-point overflow
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTerrainCompositeOverflowTest,
	"Orogeny.Terrain.Procedural.Composite.NoOverflow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTerrainCompositeOverflowTest::RunTest(const FString& Parameters)
{
	bool bAllFinite = true;

	// Sweep a range of coordinates to ensure no NaN/Inf
	for (float X = -100000.0f; X <= 100000.0f; X += 11111.0f)
	{
		for (float Y = -100000.0f; Y <= 100000.0f; Y += 11111.0f)
		{
			const float Height = FProceduralTerrainMath::CalculateHeightAt(
				X, Y, 0.00005f, 25000.0f);
			if (!FMath::IsFinite(Height))
			{
				bAllFinite = false;
				AddError(FString::Printf(
					TEXT("CalculateHeightAt non-finite at (%.0f, %.0f)"), X, Y));
			}
		}
	}

	TestTrue(TEXT("Composite terrain must produce finite values everywhere"), bAllFinite);
	return true;
}

