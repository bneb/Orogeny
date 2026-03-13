// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Iron Blight Cellular Automata Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify Von Neumann neighborhood returns exactly 4 cardinals.
// 2. Tests verify 100% probability propagates to all neighbors.
// 3. Tests verify 0% probability leaves the grid unchanged.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Blight"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "CellularBlightGrid.h"

// ============================================================================
// Test 1: Grid.Neighbors — 4 cardinal directions from origin
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlightNeighborsTest,
	"Orogeny.Blight.Grid.Neighbors",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FBlightNeighborsTest::RunTest(const FString& Parameters)
{
	TArray<FIntPoint> Neighbors = FCellularBlightGrid::GetNeighbors(FIntPoint(0, 0));

	TestEqual(TEXT("Must return exactly 4 neighbors"), Neighbors.Num(), 4);

	TestTrue(TEXT("Must contain North (0, 1)"),
		Neighbors.Contains(FIntPoint(0, 1)));
	TestTrue(TEXT("Must contain South (0, -1)"),
		Neighbors.Contains(FIntPoint(0, -1)));
	TestTrue(TEXT("Must contain East (1, 0)"),
		Neighbors.Contains(FIntPoint(1, 0)));
	TestTrue(TEXT("Must contain West (-1, 0)"),
		Neighbors.Contains(FIntPoint(-1, 0)));

	return true;
}

// ============================================================================
// Test 2: Grid.Propagation100 — 100% spread → 5 cells
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlightPropagation100Test,
	"Orogeny.Blight.Grid.Propagation100",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FBlightPropagation100Test::RunTest(const FString& Parameters)
{
	// Initialize grid with single corrupted cell at origin
	TMap<FIntPoint, bool> Grid;
	Grid.Add(FIntPoint(0, 0), true);

	// 100% spread — all 4 neighbors must become corrupted
	FCellularBlightGrid::CalculateNextGeneration(Grid, 1.0f);

	const int32 CorruptedCount = FCellularBlightGrid::CountCorrupted(Grid);
	TestEqual(TEXT("100% spread from 1 cell → 5 corrupted (center + 4 neighbors)"),
		CorruptedCount, 5);

	// Verify the specific cells
	TestTrue(TEXT("Origin (0,0) still corrupted"), Grid.FindRef(FIntPoint(0, 0)));
	TestTrue(TEXT("North (0,1) corrupted"), Grid.FindRef(FIntPoint(0, 1)));
	TestTrue(TEXT("South (0,-1) corrupted"), Grid.FindRef(FIntPoint(0, -1)));
	TestTrue(TEXT("East (1,0) corrupted"), Grid.FindRef(FIntPoint(1, 0)));
	TestTrue(TEXT("West (-1,0) corrupted"), Grid.FindRef(FIntPoint(-1, 0)));

	return true;
}

// ============================================================================
// Test 3: Grid.Propagation0 — 0% spread → no change
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBlightPropagation0Test,
	"Orogeny.Blight.Grid.Propagation0",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FBlightPropagation0Test::RunTest(const FString& Parameters)
{
	// Initialize grid with single corrupted cell at origin
	TMap<FIntPoint, bool> Grid;
	Grid.Add(FIntPoint(0, 0), true);

	// 0% spread — nothing should change
	FCellularBlightGrid::CalculateNextGeneration(Grid, 0.0f);

	const int32 CorruptedCount = FCellularBlightGrid::CountCorrupted(Grid);
	TestEqual(TEXT("0% spread → still exactly 1 corrupted cell"),
		CorruptedCount, 1);

	return true;
}
