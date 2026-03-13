// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Atmospheric Wake & VFX Data Binding Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify UTectonicAtmosphereComponent's velocity-to-radius math
//    is deterministic and correctly clamped.
// 2. The displacement radius drives fog displacement and particle intensity.
//    If this math is wrong, the Titan's atmospheric presence breaks.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Atmosphere"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "TectonicAtmosphereComponent.h"
#include "TitanCharacter.h"

// ============================================================================
// Test: Design Constants Match Constructed Defaults
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphereDesignConstantsTest,
	"Orogeny.Atmosphere.DesignConstants",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphereDesignConstantsTest::RunTest(const FString& Parameters)
{
	UTectonicAtmosphereComponent* Comp = NewObject<UTectonicAtmosphereComponent>();

	TestEqual(TEXT("Default BaseDisplacementRadius must be 5000"),
		Comp->BaseDisplacementRadius, UTectonicAtmosphereComponent::DEFAULT_BASE_RADIUS);

	TestEqual(TEXT("Default MaxVelocityMultiplier must be 2.0"),
		Comp->MaxVelocityMultiplier, UTectonicAtmosphereComponent::DEFAULT_MAX_VELOCITY_MULTIPLIER);

	TestEqual(TEXT("DEFAULT_BASE_RADIUS constant must be 5000"),
		UTectonicAtmosphereComponent::DEFAULT_BASE_RADIUS, 5000.0f);

	TestEqual(TEXT("DEFAULT_MAX_VELOCITY_MULTIPLIER constant must be 2.0"),
		UTectonicAtmosphereComponent::DEFAULT_MAX_VELOCITY_MULTIPLIER, 2.0f);

	return true;
}

// ============================================================================
// Test: Displacement Math — Stationary (Speed = 0 → BaseRadius)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphereStationaryTest,
	"Orogeny.Atmosphere.DisplacementMath.Stationary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphereStationaryTest::RunTest(const FString& Parameters)
{
	// Speed=0, MaxSpeed=150, Base=5000, Mult=2
	// Alpha = 0/150 = 0
	// Radius = 5000 * (1 + 0 * 1) = 5000
	const float Result = UTectonicAtmosphereComponent::CalculateDisplacementRadius(
		0.0f, 150.0f, 5000.0f, 2.0f);

	TestEqual(TEXT("Stationary: radius must be base (5000)"), Result, 5000.0f);

	return true;
}

// ============================================================================
// Test: Displacement Math — Max Speed (Speed = 150 → Base * Multiplier)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphereMaxSpeedTest,
	"Orogeny.Atmosphere.DisplacementMath.MaxSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphereMaxSpeedTest::RunTest(const FString& Parameters)
{
	// Speed=150, MaxSpeed=150, Base=5000, Mult=2
	// Alpha = 150/150 = 1
	// Radius = 5000 * (1 + 1 * 1) = 5000 * 2 = 10000
	const float Result = UTectonicAtmosphereComponent::CalculateDisplacementRadius(
		150.0f, 150.0f, 5000.0f, 2.0f);

	TestEqual(TEXT("Max speed: radius must be base*mult (10000)"), Result, 10000.0f);

	return true;
}

// ============================================================================
// Test: Displacement Math — Half Speed (Speed = 75 → midpoint)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphereHalfSpeedTest,
	"Orogeny.Atmosphere.DisplacementMath.HalfSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphereHalfSpeedTest::RunTest(const FString& Parameters)
{
	// Speed=75, MaxSpeed=150, Base=5000, Mult=2
	// Alpha = 75/150 = 0.5
	// Radius = 5000 * (1 + 0.5 * 1) = 5000 * 1.5 = 7500
	const float Result = UTectonicAtmosphereComponent::CalculateDisplacementRadius(
		75.0f, 150.0f, 5000.0f, 2.0f);

	TestEqual(TEXT("Half speed: radius must be 7500"), Result, 7500.0f);

	return true;
}

// ============================================================================
// Test: Displacement Math — Overspeed Clamping (Speed = 300 → capped)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphereOverspeedTest,
	"Orogeny.Atmosphere.DisplacementMath.OverspeedClamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphereOverspeedTest::RunTest(const FString& Parameters)
{
	// Speed=300, MaxSpeed=150, Base=5000, Mult=2
	// Alpha = Clamp(300/150, 0, 1) = 1.0 (clamped!)
	// Radius = 5000 * (1 + 1 * 1) = 10000
	const float Result = UTectonicAtmosphereComponent::CalculateDisplacementRadius(
		300.0f, 150.0f, 5000.0f, 2.0f);

	TestEqual(TEXT("Overspeed: radius must be clamped to max (10000)"), Result, 10000.0f);

	return true;
}

// ============================================================================
// Test: Character Integration — AtmosphereComponent exists on ATitanCharacter
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAtmosphereCharacterIntegrationTest,
	"Orogeny.Atmosphere.CharacterIntegration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAtmosphereCharacterIntegrationTest::RunTest(const FString& Parameters)
{
	ATitanCharacter* Titan = NewObject<ATitanCharacter>();

	TestNotNull(TEXT("AtmosphereComponent must exist on ATitanCharacter"),
		Titan->FindComponentByClass<UTectonicAtmosphereComponent>());

	return true;
}
