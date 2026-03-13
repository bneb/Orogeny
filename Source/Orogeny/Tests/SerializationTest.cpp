// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Save/Load Serialization Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify data packing round-trip (game state → save object).
// 2. Tests verify TMap → TArray compression (only corrupted cells).
//
// NOTE: No disk I/O in these tests. Pure data transfer only.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Serialization"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "OrogenySaveGame.h"
#include "SaveLoadSubsystem.h"

// ============================================================================
// Test 1: Data Packing — Mock values round-trip
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSerializationPackTest,
	"Orogeny.Serialization.PackSaveData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSerializationPackTest::RunTest(const FString& Parameters)
{
	UOrogenySaveGame* SaveObj = NewObject<UOrogenySaveGame>();

	const double MockDay = 500.0;
	const float MockHealth = 0.75f;
	const FVector MockLoc(100.0f, 200.0f, 300.0f);
	TArray<FIntPoint> MockBlight;
	MockBlight.Add(FIntPoint(1, 2));
	MockBlight.Add(FIntPoint(3, 4));

	USaveLoadSubsystem::PackSaveData(
		SaveObj, MockDay, MockHealth, MockLoc, MockBlight);

	TestEqual(TEXT("Day packed correctly"), SaveObj->SavedDeepTimeDay, MockDay);
	TestEqual(TEXT("Health packed correctly"), SaveObj->SavedEcosystemHealth, MockHealth);
	TestEqual(TEXT("Location packed correctly"), SaveObj->SavedTitanLocation, MockLoc);
	TestEqual(TEXT("Blight cell count"), SaveObj->SavedBlightGrid.Num(), 2);
	TestEqual(TEXT("Blight cell 0"), SaveObj->SavedBlightGrid[0], FIntPoint(1, 2));
	TestEqual(TEXT("Blight cell 1"), SaveObj->SavedBlightGrid[1], FIntPoint(3, 4));

	return true;
}

// ============================================================================
// Test 2: Grid Extraction — Empty map → 0 elements
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSerializationGridEmptyTest,
	"Orogeny.Serialization.ExtractBlightGrid.Empty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSerializationGridEmptyTest::RunTest(const FString& Parameters)
{
	TMap<FIntPoint, bool> EmptyGrid;
	TArray<FIntPoint> Result = USaveLoadSubsystem::ExtractBlightGrid(EmptyGrid);

	TestEqual(TEXT("Empty grid → 0 corrupted cells"), Result.Num(), 0);

	return true;
}

// ============================================================================
// Test 3: Grid Extraction — 3 corrupted, 2 clean → 3 elements
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSerializationGridCorruptedTest,
	"Orogeny.Serialization.ExtractBlightGrid.Corrupted",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSerializationGridCorruptedTest::RunTest(const FString& Parameters)
{
	TMap<FIntPoint, bool> Grid;
	Grid.Add(FIntPoint(0, 0), true);   // Corrupted
	Grid.Add(FIntPoint(1, 0), false);  // Clean
	Grid.Add(FIntPoint(2, 0), true);   // Corrupted
	Grid.Add(FIntPoint(3, 0), false);  // Clean
	Grid.Add(FIntPoint(4, 0), true);   // Corrupted

	TArray<FIntPoint> Result = USaveLoadSubsystem::ExtractBlightGrid(Grid);

	TestEqual(TEXT("3 corrupted out of 5 → 3 elements"), Result.Num(), 3);

	// Verify all returned cells are actually corrupted
	for (const FIntPoint& Cell : Result)
	{
		const bool* Value = Grid.Find(Cell);
		TestNotNull(TEXT("Extracted cell exists in source grid"), Value);
		if (Value)
		{
			TestTrue(TEXT("Extracted cell is corrupted"), *Value);
		}
	}

	return true;
}
