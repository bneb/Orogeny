// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Supercell Tracking Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify ASupercell_Actor's tracking interpolation is correct,
//    normalized, and prevents overshoot/jitter.
// 2. The Supercell is the primary antagonist — if tracking math is wrong,
//    the storm either teleports, jitters, or overshoots the player.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Storm"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "Components/SphereComponent.h"
#include "Supercell_Actor.h"

// ============================================================================
// Test: Design Constants Match Constructed Defaults
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSupercellDesignConstantsTest,
	"Orogeny.Storm.Supercell.DesignConstants",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSupercellDesignConstantsTest::RunTest(const FString& Parameters)
{
	ASupercell_Actor* Storm = NewObject<ASupercell_Actor>();

	TestEqual(TEXT("Default MovementSpeed must be 50"),
		Storm->MovementSpeed, ASupercell_Actor::DEFAULT_MOVEMENT_SPEED);

	TestEqual(TEXT("DEFAULT_MOVEMENT_SPEED constant must be 50"),
		ASupercell_Actor::DEFAULT_MOVEMENT_SPEED, 50.0f);

	TestEqual(TEXT("DEFAULT_STORM_CORE_RADIUS constant must be 20000"),
		ASupercell_Actor::DEFAULT_STORM_CORE_RADIUS, 20000.0f);

	return true;
}

// ============================================================================
// Test: Component Architecture — StormCore is root with correct radius
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSupercellComponentArchTest,
	"Orogeny.Storm.Supercell.ComponentArchitecture",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSupercellComponentArchTest::RunTest(const FString& Parameters)
{
	ASupercell_Actor* Storm = NewObject<ASupercell_Actor>();

	// StormCore must exist
	TestNotNull(TEXT("StormCore must be valid"), Storm->StormCore.Get());

	// StormCore must be the root component
	TestEqual(TEXT("StormCore must be the RootComponent"),
		static_cast<USceneComponent*>(Storm->StormCore.Get()),
		Storm->GetRootComponent());

	// Radius must be 20000
	if (Storm->StormCore)
	{
		TestEqual(TEXT("StormCore radius must be 20000"),
			Storm->StormCore->GetUnscaledSphereRadius(), 20000.0f);
	}

	// CloudVisuals must exist
	TestNotNull(TEXT("CloudVisuals must be valid"), Storm->CloudVisuals.Get());

	return true;
}

// ============================================================================
// Test: Tracking Math — Standard Move (straight line)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSupercellStandardMoveTest,
	"Orogeny.Storm.Supercell.Tracking.StandardMove",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSupercellStandardMoveTest::RunTest(const FString& Parameters)
{
	// Current=(0,0,0), Target=(1000,0,0), Speed=50, DeltaTime=1.0
	// Direction = (1,0,0), Step = 50
	// Result = (0,0,0) + (1,0,0) * 50 = (50, 0, 0)
	const FVector Result = ASupercell_Actor::CalculateNextPosition(
		FVector(0.0, 0.0, 0.0),
		FVector(1000.0, 0.0, 0.0),
		50.0f, 1.0f);

	TestEqual(TEXT("Standard Move: X must be 50"), Result.X, 50.0);
	TestEqual(TEXT("Standard Move: Y must be 0"), Result.Y, 0.0);
	TestEqual(TEXT("Standard Move: Z must be 0"), Result.Z, 0.0);

	return true;
}

// ============================================================================
// Test: Tracking Math — Diagonal Move (normalization proof)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSupercellDiagonalMoveTest,
	"Orogeny.Storm.Supercell.Tracking.DiagonalMove",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSupercellDiagonalMoveTest::RunTest(const FString& Parameters)
{
	// Current=(0,0,0), Target=(100,100,0), Speed=50, DeltaTime=1.0
	// Direction = Normalize(100,100,0) = (0.707, 0.707, 0)
	// Step = 50
	// Result = (0.707*50, 0.707*50, 0) = (35.35, 35.35, 0)
	// Distance from origin = sqrt(35.35^2 + 35.35^2) = 50.0
	const FVector Result = ASupercell_Actor::CalculateNextPosition(
		FVector(0.0, 0.0, 0.0),
		FVector(100.0, 100.0, 0.0),
		50.0f, 1.0f);

	// Distance traveled must be exactly 50 (normalization proof)
	const double DistanceTraveled = Result.Size();
	TestTrue(TEXT("Diagonal: distance traveled must be 50"),
		FMath::IsNearlyEqual(DistanceTraveled, 50.0, 0.01));

	// X must equal Y (45-degree diagonal)
	TestTrue(TEXT("Diagonal: X must equal Y"),
		FMath::IsNearlyEqual(Result.X, Result.Y, 0.01));

	// Z must be 0
	TestEqual(TEXT("Diagonal: Z must be 0"), Result.Z, 0.0);

	return true;
}

// ============================================================================
// Test: Tracking Math — Anti-Jitter/Overshoot (snap to target)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSupercellAntiJitterTest,
	"Orogeny.Storm.Supercell.Tracking.AntiJitter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FSupercellAntiJitterTest::RunTest(const FString& Parameters)
{
	// Current=(0,0,0), Target=(10,0,0), Speed=50, DeltaTime=1.0
	// Step=50 > Distance=10 → SNAP to target exactly
	const FVector Result = ASupercell_Actor::CalculateNextPosition(
		FVector(0.0, 0.0, 0.0),
		FVector(10.0, 0.0, 0.0),
		50.0f, 1.0f);

	TestEqual(TEXT("Anti-Jitter: X must snap to 10"), Result.X, 10.0);
	TestEqual(TEXT("Anti-Jitter: Y must be 0"), Result.Y, 0.0);
	TestEqual(TEXT("Anti-Jitter: Z must be 0"), Result.Z, 0.0);

	return true;
}
