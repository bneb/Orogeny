// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FAtmosphericMath
 *
 * Sprint 8: Pure Atmospheric Mathematics — The Breath of the World
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * Every function is static, deterministic, and world-independent.
 *
 * ARCHITECTURE:
 *   - EvaluateTimeOfDayAlpha: Sun angle → [0, 1] day/night alpha.
 *     Midnight = 0.0, Noon = 1.0. Absolute triangle wave.
 *   - CalculateFogDensity: Blight ratio scales fog from clear to choking.
 *   - BlendScatteringColor: Romantic blue → Blight amber shift.
 *
 * INTEGRATION:
 *   AAtmosphericDirector reads UDeepTimeSubsystem::CurrentSunAngle
 *   and feeds it through these functions to drive ASkyAtmosphere
 *   and AExponentialHeightFog at runtime.
 */
struct OROGENY_API FAtmosphericMath
{
	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Default base fog density (light haze) */
	static constexpr float DEFAULT_BASE_FOG_DENSITY = 0.02f;

	/** Maximum blight fog multiplier (5× = choking industrial smog) */
	static constexpr float DEFAULT_MAX_BLIGHT_MULTIPLIER = 5.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Map sun angle to a [0, 1] day/night alpha.
	 *
	 * Uses an absolute triangle wave for smooth, symmetric transitions:
	 *   0° (Midnight) = 0.0
	 *   90° (Dawn)    = 0.5
	 *   180° (Noon)   = 1.0
	 *   270° (Dusk)   = 0.5
	 *   360° (Midnight) = 0.0
	 *
	 * @param SunAngleDegrees  Current sun rotation [0, 360].
	 * @return                 Day alpha [0.0, 1.0].
	 */
	static float EvaluateTimeOfDayAlpha(float SunAngleDegrees);

	/**
	 * Calculate fog density from blight progression.
	 *
	 * Nature: clear atmospheric haze.
	 * Blight: choking industrial smog, visibility collapses.
	 *
	 * @param BaseDensity         Clear-air fog density.
	 * @param BlightRatio         [0, 1] proportion of world corrupted.
	 * @param MaxBlightMultiplier Maximum fog multiplier at full blight.
	 * @return                    Interpolated fog density.
	 */
	static float CalculateFogDensity(float BaseDensity, float BlightRatio,
		float MaxBlightMultiplier = DEFAULT_MAX_BLIGHT_MULTIPLIER);

	/**
	 * Blend sky scattering color from healthy to blighted.
	 *
	 * Romantic blue/grey → suffocating amber as blight advances.
	 *
	 * @param BaseColor    Healthy sky color (Romantic palette).
	 * @param BlightColor  Corrupted sky color (industrial amber).
	 * @param BlightRatio  [0, 1] corruption progress.
	 * @return             Blended scattering color.
	 */
	static FLinearColor BlendScatteringColor(
		const FLinearColor& BaseColor,
		const FLinearColor& BlightColor,
		float BlightRatio);
};
