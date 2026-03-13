// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FTuningMath
 *
 * Sprint 15: Pure Difficulty Mathematics — Balance Without Builds
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * Every function is static, deterministic, and world-independent.
 *
 * ARCHITECTURE:
 *   - CalculateScaledBlightProbability: Applies difficulty multiplier to
 *     base blight spread chance. Clamped [0.01, 1.0] — blight never
 *     fully stops, never exceeds 100%.
 *   - CalculateScaledEcosystemHealing: Applies difficulty multiplier to
 *     base healing rate. Clamped [0.0, 1000.0].
 *   - IsEndlessMode: Detects whether victory is disabled.
 */
struct OROGENY_API FTuningMath
{
	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Minimum blight probability — blight never fully stops */
	static constexpr float MIN_BLIGHT_PROBABILITY = 0.01f;

	/** Maximum healing rate (sanity clamp) */
	static constexpr float MAX_HEALING_RATE = 1000.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Apply difficulty multiplier to base blight spread probability.
	 * Clamped [0.01, 1.0] — blight is always present, never > 100%.
	 *
	 * @param BaseProbability       Default spread probability.
	 * @param DifficultyMultiplier  Difficulty scaling factor.
	 * @return                      Scaled probability [0.01, 1.0].
	 */
	static float CalculateScaledBlightProbability(
		float BaseProbability, float DifficultyMultiplier);

	/**
	 * Apply difficulty multiplier to base ecosystem healing rate.
	 * Clamped [0.0, 1000.0].
	 *
	 * @param BaseHealing           Default healing per tick.
	 * @param DifficultyMultiplier  Difficulty scaling factor.
	 * @return                      Scaled healing [0.0, 1000.0].
	 */
	static float CalculateScaledEcosystemHealing(
		float BaseHealing, float DifficultyMultiplier);

	/**
	 * Check if the game runs in Endless Mode (no victory condition).
	 *
	 * @param TargetCenturies  Centuries to survive: 0 = endless.
	 * @return                 True if endless mode is active.
	 */
	static bool IsEndlessMode(float TargetCenturies);
};
