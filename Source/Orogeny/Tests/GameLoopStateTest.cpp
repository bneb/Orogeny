// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Game Loop & State Management Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify exposure recovery math is perfectly clamped.
// 2. Tests verify state evaluation priority (Defeat > Victory > Playing).
// 3. If priority is wrong, a simultaneous win+loss shows victory = broken.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.GameLoop"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "OrogenyGameModeBase.h"

// ============================================================================
// Test: Exposure Math — Inside Storm (accumulation)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopExposureInsideTest,
	"Orogeny.GameLoop.Exposure.Inside",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopExposureInsideTest::RunTest(const FString& Parameters)
{
	// Current=5.0, inside, DeltaTime=1.0, Max=10.0 → 6.0
	const float Result = AOrogenyGameModeBase::CalculateExposureDelta(
		5.0f, true, 1.0f, 10.0f);
	TestEqual(TEXT("Inside storm: 5.0 + 1.0 = 6.0"), Result, 6.0f);
	return true;
}

// ============================================================================
// Test: Exposure Math — Recovery (outside storm)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopExposureRecoveryTest,
	"Orogeny.GameLoop.Exposure.Recovery",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopExposureRecoveryTest::RunTest(const FString& Parameters)
{
	// Current=5.0, outside, DeltaTime=1.0, Max=10.0 → 4.0
	const float Result = AOrogenyGameModeBase::CalculateExposureDelta(
		5.0f, false, 1.0f, 10.0f);
	TestEqual(TEXT("Recovery: 5.0 - 1.0 = 4.0"), Result, 4.0f);
	return true;
}

// ============================================================================
// Test: Exposure Math — Clamping (both directions)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopExposureClampTest,
	"Orogeny.GameLoop.Exposure.Clamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopExposureClampTest::RunTest(const FString& Parameters)
{
	// Overshoot ceiling: 9.5 + 1.0 → clamped to 10.0
	const float CeilResult = AOrogenyGameModeBase::CalculateExposureDelta(
		9.5f, true, 1.0f, 10.0f);
	TestEqual(TEXT("Ceiling clamp: 9.5 + 1.0 = 10.0"), CeilResult, 10.0f);

	// Undershoot floor: 0.5 - 1.0 → clamped to 0.0
	const float FloorResult = AOrogenyGameModeBase::CalculateExposureDelta(
		0.5f, false, 1.0f, 10.0f);
	TestEqual(TEXT("Floor clamp: 0.5 - 1.0 = 0.0"), FloorResult, 0.0f);

	return true;
}

// ============================================================================
// Test: State Evaluation — Victory
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopVictoryTest,
	"Orogeny.GameLoop.State.Victory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopVictoryTest::RunTest(const FString& Parameters)
{
	// Survived 900s, required 900s, no exposure
	const EOrogenyGameState Result = AOrogenyGameModeBase::EvaluateGameState(
		900.0f, 900.0f, 0.0f, 10.0f);
	TestEqual(TEXT("Survived required time: Victory"),
		static_cast<uint8>(Result),
		static_cast<uint8>(EOrogenyGameState::Victory));
	return true;
}

// ============================================================================
// Test: State Evaluation — Defeat
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopDefeatTest,
	"Orogeny.GameLoop.State.Defeat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopDefeatTest::RunTest(const FString& Parameters)
{
	// Only 100s survived, but exposure maxed
	const EOrogenyGameState Result = AOrogenyGameModeBase::EvaluateGameState(
		100.0f, 900.0f, 10.0f, 10.0f);
	TestEqual(TEXT("Storm exposure maxed: Defeat"),
		static_cast<uint8>(Result),
		static_cast<uint8>(EOrogenyGameState::Defeat));
	return true;
}

// ============================================================================
// Test: State Evaluation — Priority (Defeat ALWAYS overrides Victory)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopPriorityTest,
	"Orogeny.GameLoop.State.DefeatPriority",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopPriorityTest::RunTest(const FString& Parameters)
{
	// Both conditions met simultaneously: survival AND exposure maxed
	// Defeat MUST take priority — the storm claims all
	const EOrogenyGameState Result = AOrogenyGameModeBase::EvaluateGameState(
		900.0f, 900.0f, 10.0f, 10.0f);
	TestEqual(TEXT("Simultaneous win+loss: Defeat takes priority"),
		static_cast<uint8>(Result),
		static_cast<uint8>(EOrogenyGameState::Defeat));
	return true;
}

// ============================================================================
// Sprint 5: Deep Time State Evaluation — Victory (endured 10 centuries)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopDeepTimeVictoryTest,
	"Orogeny.GameLoop.DeepTime.Victory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopDeepTimeVictoryTest::RunTest(const FString& Parameters)
{
	// 10 centuries survived, target 10, health at 0.5 (alive), critical 0.0
	const EOrogenyGameState Result = AOrogenyGameModeBase::EvaluateDeepTimeGameState(
		10.0f, 10.0f, 0.5f, 0.0f);
	TestEqual(TEXT("Endured 10 centuries with health > 0: Victory"),
		static_cast<uint8>(Result),
		static_cast<uint8>(EOrogenyGameState::Victory));
	return true;
}

// ============================================================================
// Sprint 5: Deep Time State Evaluation — Ecosystem Defeat (health = 0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopDeepTimeDefeatTest,
	"Orogeny.GameLoop.DeepTime.EcosystemDefeat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopDeepTimeDefeatTest::RunTest(const FString& Parameters)
{
	// Only 5 centuries survived, health at 0.0 = bare mountain
	const EOrogenyGameState Result = AOrogenyGameModeBase::EvaluateDeepTimeGameState(
		5.0f, 10.0f, 0.0f, 0.0f);
	TestEqual(TEXT("Ecosystem health at 0: Defeat"),
		static_cast<uint8>(Result),
		static_cast<uint8>(EOrogenyGameState::Defeat));
	return true;
}

// ============================================================================
// Sprint 5: Deep Time — Priority (Defeat overrides Victory at boundary)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameLoopDeepTimePriorityTest,
	"Orogeny.GameLoop.DeepTime.DefeatPriority",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FGameLoopDeepTimePriorityTest::RunTest(const FString& Parameters)
{
	// 10 centuries AND health = 0 simultaneously → Defeat wins
	const EOrogenyGameState Result = AOrogenyGameModeBase::EvaluateDeepTimeGameState(
		10.0f, 10.0f, 0.0f, 0.0f);
	TestEqual(TEXT("Victory + bare mountain simultaneously: Defeat takes priority"),
		static_cast<uint8>(Result),
		static_cast<uint8>(EOrogenyGameState::Defeat));
	return true;
}
