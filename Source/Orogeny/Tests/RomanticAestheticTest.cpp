// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Romantic Aesthetic Enforcement Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify that the Romantic-era color profile is EXACTLY locked
//    in the PostProcessComponent settings struct.
// 2. If any value drifts, the entire visual identity is compromised.
// 3. Override flags MUST be true or the engine silently ignores the values.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Aesthetic"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "Components/PostProcessComponent.h"
#include "RomanticAtmosphereDirector.h"

// ============================================================================
// Test: Component Architecture — unbound, priority 100
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAestheticArchTest,
	"Orogeny.Aesthetic.ComponentArchitecture",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAestheticArchTest::RunTest(const FString& Parameters)
{
	ARomanticAtmosphereDirector* Director = NewObject<ARomanticAtmosphereDirector>();

	// PostProcessVolume must exist
	TestNotNull(TEXT("PostProcessVolume must be valid"),
		Director->PostProcessVolume.Get());

	if (Director->PostProcessVolume)
	{
		// Must be unbound (global)
		TestTrue(TEXT("PostProcessVolume must be unbound"),
			Director->PostProcessVolume->bUnbound);

		// Must be priority 100
		TestEqual(TEXT("Priority must be 100"),
			Director->PostProcessVolume->Priority,
			ARomanticAtmosphereDirector::PP_PRIORITY);
	}

	return true;
}

// ============================================================================
// Test: Contrast & Saturation Lock
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAestheticContrastSatTest,
	"Orogeny.Aesthetic.ContrastSaturationLock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAestheticContrastSatTest::RunTest(const FString& Parameters)
{
	ARomanticAtmosphereDirector* Director = NewObject<ARomanticAtmosphereDirector>();

	if (!Director->PostProcessVolume)
	{
		AddError(TEXT("PostProcessVolume is null"));
		return false;
	}

	const FPostProcessSettings& S = Director->PostProcessVolume->Settings;

	// Contrast must be 1.25
	TestEqual(TEXT("ColorContrast.X must be 1.25"),
		static_cast<float>(S.ColorContrast.X), ARomanticAtmosphereDirector::CONTRAST);

	// Saturation must be 0.85
	TestEqual(TEXT("ColorSaturation.X must be 0.85"),
		static_cast<float>(S.ColorSaturation.X), ARomanticAtmosphereDirector::SATURATION);

	return true;
}

// ============================================================================
// Test: Split Toning Lock — shadows cold, highlights warm
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAestheticSplitToningTest,
	"Orogeny.Aesthetic.SplitToningLock",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAestheticSplitToningTest::RunTest(const FString& Parameters)
{
	ARomanticAtmosphereDirector* Director = NewObject<ARomanticAtmosphereDirector>();

	if (!Director->PostProcessVolume)
	{
		AddError(TEXT("PostProcessVolume is null"));
		return false;
	}

	const FPostProcessSettings& S = Director->PostProcessVolume->Settings;

	// Shadows must favor blue: Z > X
	TestTrue(TEXT("Shadow gamma Z (blue) must exceed X (red)"),
		S.ColorGammaShadows.Z > S.ColorGammaShadows.X);

	// Highlights must favor warmth: X > Z
	TestTrue(TEXT("Highlight gamma X (red) must exceed Z (blue)"),
		S.ColorGammaHighlights.X > S.ColorGammaHighlights.Z);

	return true;
}

// ============================================================================
// Test: Override Flags — all must be true
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAestheticOverrideFlagsTest,
	"Orogeny.Aesthetic.OverrideFlags",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FAestheticOverrideFlagsTest::RunTest(const FString& Parameters)
{
	ARomanticAtmosphereDirector* Director = NewObject<ARomanticAtmosphereDirector>();

	if (!Director->PostProcessVolume)
	{
		AddError(TEXT("PostProcessVolume is null"));
		return false;
	}

	const FPostProcessSettings& S = Director->PostProcessVolume->Settings;

	TestTrue(TEXT("bOverride_ColorContrast must be true"),
		S.bOverride_ColorContrast);
	TestTrue(TEXT("bOverride_ColorSaturation must be true"),
		S.bOverride_ColorSaturation);
	TestTrue(TEXT("bOverride_ColorGammaShadows must be true"),
		S.bOverride_ColorGammaShadows);
	TestTrue(TEXT("bOverride_ColorGammaHighlights must be true"),
		S.bOverride_ColorGammaHighlights);
	TestTrue(TEXT("bOverride_VignetteIntensity must be true"),
		S.bOverride_VignetteIntensity);
	TestTrue(TEXT("bOverride_FilmGrainIntensity must be true"),
		S.bOverride_FilmGrainIntensity);

	return true;
}
