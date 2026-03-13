// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Deep Time Chrono-Tick Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify the velocity→time-scale mapping is exactly correct.
// 2. Tests verify day advancement math at both base and max rates.
// 3. Tests verify sun angle wraps correctly at whole-day boundaries.
// 4. Tests verify seasonal cycling over the 365.25-day period.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.DeepTime"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "DeepTimeSubsystem.h"

// ============================================================================
// Test 1: TimeScale.Stationary — alpha=0 → scale=1.0
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeScaleStationaryTest,
	"Orogeny.DeepTime.TimeScale.Stationary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeScaleStationaryTest::RunTest(const FString& Parameters)
{
	const float Result = UDeepTimeSubsystem::CalculateTimeScale(0.0f);
	TestEqual(TEXT("Stationary: TimeScale must be 1.0"), Result, 1.0f);
	return true;
}

// ============================================================================
// Test 2: TimeScale.MaxSpeed — alpha=1 → scale=36525.0
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeScaleMaxTest,
	"Orogeny.DeepTime.TimeScale.MaxSpeed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeScaleMaxTest::RunTest(const FString& Parameters)
{
	const float Result = UDeepTimeSubsystem::CalculateTimeScale(1.0f);
	TestEqual(TEXT("MaxSpeed: TimeScale must be 36525.0"),
		Result, UDeepTimeSubsystem::MAX_TIME_SCALE);
	return true;
}

// ============================================================================
// Test 3: TimeScale.Clamping — overshoot and undershoot
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeScaleClampTest,
	"Orogeny.DeepTime.TimeScale.Clamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeScaleClampTest::RunTest(const FString& Parameters)
{
	// Overshoot: alpha=2.0 → clamped to 1.0 → MAX_TIME_SCALE
	const float Over = UDeepTimeSubsystem::CalculateTimeScale(2.0f);
	TestEqual(TEXT("Overshoot clamped to MAX_TIME_SCALE"),
		Over, UDeepTimeSubsystem::MAX_TIME_SCALE);

	// Undershoot: alpha=-1.0 → clamped to 0.0 → 1.0
	const float Under = UDeepTimeSubsystem::CalculateTimeScale(-1.0f);
	TestEqual(TEXT("Undershoot clamped to 1.0"), Under, 1.0f);

	return true;
}

// ============================================================================
// Test 4: AdvanceDay.BaseRate — 1 second at scale=1 → 1/60 day
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeAdvanceBaseTest,
	"Orogeny.DeepTime.AdvanceDay.BaseRate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeAdvanceBaseTest::RunTest(const FString& Parameters)
{
	// TimeScale=1, Δt=1s, BaseDayDuration=60 → 1/60 days
	const double Result = UDeepTimeSubsystem::AdvanceDay(0.0, 1.0f, 1.0f, 60.0f);
	const double Expected = 1.0 / 60.0;

	TestTrue(TEXT("Base rate: 1 second → 1/60 day"),
		FMath::IsNearlyEqual(Result, Expected, 1e-6));
	return true;
}

// ============================================================================
// Test 5: AdvanceDay.MaxAcceleration — 1 second at max → 608.75 days
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeAdvanceMaxTest,
	"Orogeny.DeepTime.AdvanceDay.MaxAcceleration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeAdvanceMaxTest::RunTest(const FString& Parameters)
{
	// TimeScale=36525, Δt=1s, BaseDayDuration=60 → 36525/60 = 608.75 days
	const double Result = UDeepTimeSubsystem::AdvanceDay(
		0.0, UDeepTimeSubsystem::MAX_TIME_SCALE, 1.0f, 60.0f);
	const double Expected = 608.75;

	TestTrue(TEXT("Max acceleration: 1 second → 608.75 days"),
		FMath::IsNearlyEqual(Result, Expected, 0.01));
	return true;
}

// ============================================================================
// Test 6: SunAngle.Midnight — wraps at whole days
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeSunMidnightTest,
	"Orogeny.DeepTime.SunAngle.Midnight",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeSunMidnightTest::RunTest(const FString& Parameters)
{
	// Day 0.0 = midnight = 0°
	TestTrue(TEXT("Day 0.0 → 0°"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSunAngle(0.0), 0.0f, 0.01f));

	// Day 1.0 = whole day wraps → 0°
	TestTrue(TEXT("Day 1.0 → 0° (wrap)"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSunAngle(1.0), 0.0f, 0.01f));

	// Day 100.0 = 100 whole days → 0°
	TestTrue(TEXT("Day 100.0 → 0° (wrap)"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSunAngle(100.0), 0.0f, 0.01f));

	return true;
}

// ============================================================================
// Test 7: SunAngle.Noon — half day = 180°
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeSunNoonTest,
	"Orogeny.DeepTime.SunAngle.Noon",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeSunNoonTest::RunTest(const FString& Parameters)
{
	// Day 0.5 = noon = 180°
	TestTrue(TEXT("Day 0.5 → 180°"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSunAngle(0.5), 180.0f, 0.01f));

	// Day 1.5 = noon of day 2 = 180°
	TestTrue(TEXT("Day 1.5 → 180°"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSunAngle(1.5), 180.0f, 0.01f));

	// Day 365.5 = noon of day 366 = 180°
	TestTrue(TEXT("Day 365.5 → 180°"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSunAngle(365.5), 180.0f, 0.01f));

	return true;
}

// ============================================================================
// Test 8: SeasonAlpha.Cycle — 365.25-day period
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FDeepTimeSeasonCycleTest,
	"Orogeny.DeepTime.SeasonAlpha.Cycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FDeepTimeSeasonCycleTest::RunTest(const FString& Parameters)
{
	// Day 0 → winter solstice → 0.0
	TestTrue(TEXT("Day 0 → SeasonAlpha 0.0 (winter solstice)"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSeasonAlpha(0.0), 0.0f, 1e-5f));

	// Day 365.25 → full cycle → wraps to 0.0
	TestTrue(TEXT("Day 365.25 → SeasonAlpha 0.0 (full year wrap)"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSeasonAlpha(365.25), 0.0f, 1e-5f));

	// Day 365.25/2 = 182.625 → summer solstice → 0.5
	TestTrue(TEXT("Day 182.625 → SeasonAlpha 0.5 (summer solstice)"),
		FMath::IsNearlyEqual(UDeepTimeSubsystem::CalculateSeasonAlpha(365.25 / 2.0), 0.5f, 1e-5f));

	return true;
}
