// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FFeedbackMath
 *
 * Sprint 11: Pure Feedback Mathematics — The Titan's Weight
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * Every function is static, deterministic, and world-independent.
 *
 * ARCHITECTURE:
 *   - CalculateFootstepShakeScale: Velocity → shake intensity.
 *     Standing still = no shake. Full sprint = full heave.
 *   - CalculateHealthVignetteWeight: Health → vignette darkness.
 *     Only appears below critical threshold (dying mountain).
 *   - CalculateSubductionRumbleIntensity: Depth → rumble strength.
 *     Only active during transitions (grinding into earth).
 */
struct OROGENY_API FFeedbackMath
{
	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Minimum velocity alpha before shake activates */
	static constexpr float DEFAULT_MIN_SPEED_ALPHA = 0.1f;

	/** Health ratio below which vignette starts */
	static constexpr float DEFAULT_CRITICAL_THRESHOLD = 0.3f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Map velocity alpha to footstep camera shake scale.
	 *
	 * Below MinSpeedAlpha: no shake (standing still).
	 * [MinSpeedAlpha, 1.0] maps to [0.1, 1.0] shake intensity.
	 *
	 * @param VelocityAlpha   Normalized velocity [0, 1].
	 * @param MinSpeedAlpha   Threshold below which shake is zero.
	 * @return                Shake scale [0.0, 1.0].
	 */
	static float CalculateFootstepShakeScale(float VelocityAlpha,
		float MinSpeedAlpha = DEFAULT_MIN_SPEED_ALPHA);

	/**
	 * Map ecosystem health to diegetic vignette weight.
	 *
	 * Above CriticalThreshold: no vignette (healthy mountain).
	 * [CriticalThreshold, 0.0] maps to [0.0, 1.0] vignette darkness.
	 *
	 * @param EcosystemHealth   Current health.
	 * @param MaxHealth         Maximum health (normalization).
	 * @param CriticalThreshold Health ratio below which vignette appears.
	 * @return                  Vignette weight [0.0, 1.0].
	 */
	static float CalculateHealthVignetteWeight(float EcosystemHealth,
		float MaxHealth = 1.0f, float CriticalThreshold = DEFAULT_CRITICAL_THRESHOLD);

	/**
	 * Map subduction depth to controller rumble intensity.
	 *
	 * Only active during transitions (grinding into earth).
	 * Maps depth [0, MaxDepth] to intensity [0.5, 1.0].
	 *
	 * @param bIsTransitioning  True if subduction is in progress.
	 * @param CurrentDepth      Current Z offset (negative = deeper).
	 * @param MaxDepth          Maximum subduction depth (negative).
	 * @return                  Rumble intensity [0.0, 1.0].
	 */
	static float CalculateSubductionRumbleIntensity(bool bIsTransitioning,
		float CurrentDepth, float MaxDepth);
};
