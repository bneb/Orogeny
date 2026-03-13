// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Audio Data Pipeline Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify UTectonicAudioComponent's velocity-to-alpha math
//    is perfectly normalized and clamped [0, 1].
// 2. Any value outside this range would blow out MetaSound graphs
//    (LFO frequency, sub-bass volume, grinding noise level).
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Audio"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "TectonicAudioComponent.h"

// ============================================================================
// Test: Speed Alpha — Stationary (Speed=0 → 0.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAudioStationaryTest,
	"Orogeny.Audio.SpeedAlpha.Stationary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAudioStationaryTest::RunTest(const FString& Parameters)
{
	const float Result = UTectonicAudioComponent::CalculateSpeedAlpha(0.0f, 150.0f);
	TestEqual(TEXT("Stationary: alpha must be 0.0"), Result, 0.0f);
	return true;
}

// ============================================================================
// Test: Speed Alpha — Max Speed (Speed=150 → 1.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAudioMaxSpeedTest,
	"Orogeny.Audio.SpeedAlpha.MaxSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAudioMaxSpeedTest::RunTest(const FString& Parameters)
{
	const float Result = UTectonicAudioComponent::CalculateSpeedAlpha(150.0f, 150.0f);
	TestEqual(TEXT("Max speed: alpha must be 1.0"), Result, 1.0f);
	return true;
}

// ============================================================================
// Test: Speed Alpha — Clamping (Speed=300 → 1.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAudioClampingTest,
	"Orogeny.Audio.SpeedAlpha.Clamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAudioClampingTest::RunTest(const FString& Parameters)
{
	const float Result = UTectonicAudioComponent::CalculateSpeedAlpha(300.0f, 150.0f);
	TestEqual(TEXT("Overspeed: alpha must clamp to 1.0"), Result, 1.0f);
	return true;
}

// ============================================================================
// Test: Component Architecture — Default SpeedParameterName
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAudioComponentArchTest,
	"Orogeny.Audio.ComponentArchitecture",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAudioComponentArchTest::RunTest(const FString& Parameters)
{
	UTectonicAudioComponent* Comp = NewObject<UTectonicAudioComponent>();

	TestEqual(TEXT("SpeedParameterName must be 'SpeedAlpha'"),
		Comp->SpeedParameterName, FName("SpeedAlpha"));

	TestEqual(TEXT("SILENCE_THRESHOLD must be 0.01"),
		UTectonicAudioComponent::SILENCE_THRESHOLD, 0.01f);

	return true;
}
