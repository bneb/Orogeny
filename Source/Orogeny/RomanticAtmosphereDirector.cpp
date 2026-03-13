// Copyright Orogeny. All Rights Reserved.

#include "RomanticAtmosphereDirector.h"
#include "Orogeny.h"
#include "Components/PostProcessComponent.h"

// ============================================================================
// Constructor — The Aesthetic Hardcode
// ============================================================================
// Every value here is a deliberate artistic choice.
// Do not expose these as EditAnywhere — the profile is LOCKED.
//
// Cost: Zero additional GPU shaders. This uses UE5's built-in filmic
// tonemapper which runs regardless. We are merely configuring its parameters.
// ============================================================================

ARomanticAtmosphereDirector::ARomanticAtmosphereDirector()
{
	PrimaryActorTick.bCanEverTick = false;

	// -----------------------------------------------------------------------
	// Post Process Volume — unbound, global, maximum priority
	// -----------------------------------------------------------------------
	PostProcessVolume = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessVolume"));
	PostProcessVolume->bUnbound = true;
	PostProcessVolume->Priority = PP_PRIORITY;
	SetRootComponent(PostProcessVolume);

	// -----------------------------------------------------------------------
	// Color Grading — The Romantic Palette
	// -----------------------------------------------------------------------
	FPostProcessSettings& Settings = PostProcessVolume->Settings;

	// -- Global Contrast: 1.25 — crush midtones, stark silhouettes ----------
	Settings.bOverride_ColorContrast = true;
	Settings.ColorContrast = FVector4(CONTRAST, CONTRAST, CONTRAST, 1.0f);

	// -- Global Saturation: 0.85 — muted, solemn world ---------------------
	Settings.bOverride_ColorSaturation = true;
	Settings.ColorSaturation = FVector4(SATURATION, SATURATION, SATURATION, 1.0f);

	// -- Split Toning: Shadows — cold brooding blues (Z > X) ---------------
	Settings.bOverride_ColorGammaShadows = true;
	Settings.ColorGammaShadows = FVector4(SHADOW_R, SHADOW_G, SHADOW_B, 1.0f);

	// -- Split Toning: Highlights — warm piercing oranges (X > Z) ----------
	Settings.bOverride_ColorGammaHighlights = true;
	Settings.ColorGammaHighlights = FVector4(HIGHLIGHT_R, HIGHLIGHT_G, HIGHLIGHT_B, 1.0f);

	// -- Vignette: 0.8 — heavy edge darkening ------------------------------
	Settings.bOverride_VignetteIntensity = true;
	Settings.VignetteIntensity = VIGNETTE;

	// -- Film Grain: 0.15 — subtle canvas texture --------------------------
	Settings.bOverride_FilmGrainIntensity = true;
	Settings.FilmGrainIntensity = FILM_GRAIN;

	UE_LOG(LogOrogeny, Log,
		TEXT("ARomanticAtmosphereDirector: Aesthetic locked. "
		     "Contrast=%.2f, Saturation=%.2f, Vignette=%.2f, Grain=%.2f, Priority=%.0f"),
		CONTRAST, SATURATION, VIGNETTE, FILM_GRAIN, PP_PRIORITY);
}
