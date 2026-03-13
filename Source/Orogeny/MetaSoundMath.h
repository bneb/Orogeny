// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FMetaSoundMath
 *
 * Sprint 10: Pure Acoustic Mathematics — The Voice of the World
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * Every function is static, deterministic, and world-independent.
 *
 * ARCHITECTURE:
 *   - CalculateWindIntensity: Altitude → [0, 1] wind parameter.
 *     Valleys are still; peaks are howling.
 *   - CalculateFaunaVolume: Ecosystem health → [0, 1] chorus volume.
 *     Healthy forest = rich birdsong; dead = silence.
 *   - CalculateTimeCompressionPitch: Deep Time scale → clamped pitch.
 *     Logarithmic compression prevents DSP chaos at 36,525x.
 *
 * INTEGRATION:
 *   AMetaSoundDirector reads game state each tick and feeds these
 *   normalized floats into UAudioComponent::SetFloatParameter().
 */
struct OROGENY_API FMetaSoundMath
{
	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Valley floor altitude (cm) — minimum wind */
	static constexpr float DEFAULT_VALLEY_Z = 5000.0f;

	/** Mountain peak altitude (cm) — maximum wind */
	static constexpr float DEFAULT_PEAK_Z = 25000.0f;

	/** Deep Time base time scale (1x real-time) */
	static constexpr float DEFAULT_BASE_SCALE = 1.0f;

	/** Deep Time max time scale (1 century per second) */
	static constexpr float DEFAULT_MAX_SCALE = 36525.0f;

	/** Maximum pitch multiplier (prevents DSP distortion) */
	static constexpr float DEFAULT_MAX_PITCH = 3.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Normalize altitude to wind intensity [0, 1].
	 *
	 * Valley = calm (0.0). Peak = howling gale (1.0).
	 * Clamped: below valley = 0, above peak = 1.
	 *
	 * @param ZHeight  Current altitude (cm).
	 * @param ValleyZ  Altitude floor (0% wind).
	 * @param PeakZ    Altitude ceiling (100% wind).
	 * @return         Wind intensity [0.0, 1.0].
	 */
	static float CalculateWindIntensity(float ZHeight,
		float ValleyZ = DEFAULT_VALLEY_Z, float PeakZ = DEFAULT_PEAK_Z);

	/**
	 * Normalize ecosystem health to fauna chorus volume [0, 1].
	 *
	 * Linear pass-through — MetaSound applies easing curves.
	 * Healthy = rich birdsong. Dead = silence.
	 *
	 * @param EcosystemHealth  Current health value.
	 * @param MaxHealth        Maximum health (normalization denominator).
	 * @return                 Volume [0.0, 1.0].
	 */
	static float CalculateFaunaVolume(float EcosystemHealth,
		float MaxHealth = 1.0f);

	/**
	 * Compress Deep Time scale to safe pitch multiplier.
	 *
	 * Logarithmic curve: linear pitch at 36,525x would destroy DSP.
	 * log(CurrentScale) / log(MaxScale) → [0, 1] → lerp to MaxPitch.
	 *
	 * @param CurrentTimeScale  Current time acceleration factor.
	 * @param BaseScale         Normal speed (1.0x).
	 * @param MaxScale          Maximum Deep Time speed.
	 * @param MaxPitch          Maximum safe pitch multiplier.
	 * @return                  Pitch multiplier [1.0, MaxPitch].
	 */
	static float CalculateTimeCompressionPitch(float CurrentTimeScale,
		float BaseScale = DEFAULT_BASE_SCALE,
		float MaxScale = DEFAULT_MAX_SCALE,
		float MaxPitch = DEFAULT_MAX_PITCH);
};
