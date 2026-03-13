// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RomanticAtmosphereDirector.generated.h"

class UPostProcessComponent;

/**
 * ARomanticAtmosphereDirector
 *
 * Day 11: The Aesthetic Lock — Caspar David Friedrich Color Profile
 *
 * A global, unbound Post Process Volume that enforces the Romantic-era
 * visual identity across the entire world. No custom shaders — pure
 * UE5 filmic tonemapper parameters.
 *
 * DESIGN PHILOSOPHY:
 *   Stark contrasts. Brooding skies. Heavy silhouettes. Profound isolation.
 *   The Kuwahara oil-paint filter was cut on Day 1 to protect the 780M's
 *   frame budget. This replaces it with pure art direction at zero GPU cost.
 *
 * ENFORCEMENT:
 *   Priority = 100 overrides any level designer's accidental tweaks.
 *   bUnbound = true applies globally. Values are hardcoded constexpr.
 *
 * COLOR GRADING:
 *   Contrast:   1.25 — crush midtones, stark silhouettes
 *   Saturation: 0.85 — muted, solemn world
 *   Shadows:    cold blue (Z > X) — brooding depth
 *   Highlights: warm orange (X > Z) — piercing god-rays
 *   Vignette:   0.8 — heavy edge darkening
 *   Film Grain: 0.15 — subtle canvas texture
 */
UCLASS()
class OROGENY_API ARomanticAtmosphereDirector : public AActor
{
	GENERATED_BODY()

public:
	ARomanticAtmosphereDirector();

	// -----------------------------------------------------------------------
	// Design Constants — the Romantic Palette
	// -----------------------------------------------------------------------

	/** Global contrast: crush midtones for stark silhouettes */
	static constexpr float CONTRAST = 1.25f;

	/** Global saturation: muted, solemn world */
	static constexpr float SATURATION = 0.85f;

	/** Shadow gamma — cold brooding blues (R, G, B, A) */
	static constexpr float SHADOW_R = 0.9f;
	static constexpr float SHADOW_G = 0.9f;
	static constexpr float SHADOW_B = 1.1f;

	/** Highlight gamma — warm piercing oranges (R, G, B, A) */
	static constexpr float HIGHLIGHT_R = 1.1f;
	static constexpr float HIGHLIGHT_G = 1.05f;
	static constexpr float HIGHLIGHT_B = 0.9f;

	/** Vignette: heavy edge darkening to draw eye to the mountain */
	static constexpr float VIGNETTE = 0.8f;

	/** Film grain: subtle canvas texture to evoke oil painting */
	static constexpr float FILM_GRAIN = 0.15f;

	/** Post process priority: overrides all other volumes */
	static constexpr float PP_PRIORITY = 100.0f;

	// -----------------------------------------------------------------------
	// Components
	// -----------------------------------------------------------------------

	/** The unbound post process volume enforcing the aesthetic globally. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Aesthetic")
	TObjectPtr<UPostProcessComponent> PostProcessVolume;
};
