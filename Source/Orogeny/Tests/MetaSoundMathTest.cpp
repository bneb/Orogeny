// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: MetaSound Acoustic Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify altitude → wind intensity mapping.
// 2. Tests verify health → fauna volume mapping.
// 3. Tests verify logarithmic time compression pitch.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Audio"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "MetaSoundMath.h"

// ============================================================================
// Test 1: Wind Intensity - Deep Valley (0 cm → 0.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMetaSoundWindValleyTest,
	"Orogeny.Audio.MetaSound.Wind.Valley",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FMetaSoundWindValleyTest::RunTest(const FString& Parameters)
{
	const float Wind = FMetaSoundMath::CalculateWindIntensity(0.0f);
	TestEqual(TEXT("Valley floor (0 cm) = 0.0 wind"), Wind, 0.0f);
	return true;
}

// ============================================================================
// Test 2: Wind Intensity - Midpoint (15000 cm → 0.5)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMetaSoundWindMidTest,
	"Orogeny.Audio.MetaSound.Wind.Midpoint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FMetaSoundWindMidTest::RunTest(const FString& Parameters)
{
	const float Wind = FMetaSoundMath::CalculateWindIntensity(15000.0f);
	TestEqual(TEXT("Midpoint (15000 cm) = 0.5 wind"), Wind, 0.5f);
	return true;
}

// ============================================================================
// Test 3: Wind Intensity - High Peak (30000 cm → 1.0, clamped)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMetaSoundWindPeakTest,
	"Orogeny.Audio.MetaSound.Wind.Peak",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FMetaSoundWindPeakTest::RunTest(const FString& Parameters)
{
	const float Wind = FMetaSoundMath::CalculateWindIntensity(30000.0f);
	TestEqual(TEXT("Peak (30000 cm) = 1.0 wind (clamped)"), Wind, 1.0f);
	return true;
}

// ============================================================================
// Test 4: Fauna Volume - Dead / Alive
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMetaSoundFaunaTest,
	"Orogeny.Audio.MetaSound.Fauna.DeadAlive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FMetaSoundFaunaTest::RunTest(const FString& Parameters)
{
	const float Dead = FMetaSoundMath::CalculateFaunaVolume(0.0f);
	const float Alive = FMetaSoundMath::CalculateFaunaVolume(1.0f);

	TestEqual(TEXT("Dead ecosystem = 0.0 volume"), Dead, 0.0f);
	TestEqual(TEXT("Healthy ecosystem = 1.0 volume"), Alive, 1.0f);
	return true;
}

// ============================================================================
// Test 5: Time Pitch - Base Speed (1.0x → pitch 1.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMetaSoundPitchBaseTest,
	"Orogeny.Audio.MetaSound.Pitch.BaseSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FMetaSoundPitchBaseTest::RunTest(const FString& Parameters)
{
	const float Pitch = FMetaSoundMath::CalculateTimeCompressionPitch(1.0f);
	TestEqual(TEXT("Base speed (1x) = pitch 1.0"), Pitch, 1.0f);
	return true;
}

// ============================================================================
// Test 6: Time Pitch - Max Speed (36525x → pitch 3.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMetaSoundPitchMaxTest,
	"Orogeny.Audio.MetaSound.Pitch.MaxSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FMetaSoundPitchMaxTest::RunTest(const FString& Parameters)
{
	const float Pitch = FMetaSoundMath::CalculateTimeCompressionPitch(36525.0f);
	TestEqual(TEXT("Max Deep Time (36525x) = pitch 3.0"), Pitch, 3.0f);
	return true;
}
