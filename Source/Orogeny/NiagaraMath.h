// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FNiagaraMath
 *
 * Sprint 13: Pure VFX Mathematics — The Mountain's Breath
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * Every function is static, deterministic, and world-independent.
 *
 * ARCHITECTURE:
 *   - CalculateDustSpawnRate: Velocity → particle spawn rate.
 *     Stationary = no dust. Full sprint = dust storm.
 *   - CalculateDebrisBurstCount: Depth → rock explosion count.
 *     Surface = nothing. Max depth = avalanche.
 *   - CalculateSmokeOpacity: Blight → smoke density.
 *     Clean = invisible. Full blight = choking.
 *
 * BUDGET (780M iGPU):
 *   MaxSpawnRate capped at 500 to keep GPU particle count under 2K.
 *   Debris bursts are one-shot, not sustained.
 */
struct OROGENY_API FNiagaraMath
{
	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Max footstep dust particles per second */
	static constexpr float DEFAULT_MAX_DUST_SPAWN_RATE = 500.0f;

	/** Max subduction burst particle count */
	static constexpr int32 DEFAULT_MAX_DEBRIS_BURST = 1000;

	/** Default max depth for subduction (cm, negative) */
	static constexpr float DEFAULT_MAX_DEPTH = -15000.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Map velocity alpha to footstep dust spawn rate.
	 *
	 * Still = 0 particles. Max speed = MaxSpawnRate.
	 *
	 * @param VelocityAlpha  Normalized velocity [0, 1].
	 * @param MaxSpawnRate   Maximum particles per second.
	 * @return               Spawn rate [0, MaxSpawnRate].
	 */
	static float CalculateDustSpawnRate(float VelocityAlpha,
		float MaxSpawnRate = DEFAULT_MAX_DUST_SPAWN_RATE);

	/**
	 * Map subduction depth to debris burst particle count.
	 *
	 * Surface = 0 particles. Full depth = MaxBurst avalanche.
	 *
	 * @param SubductionDepth  Current depth (negative = deeper).
	 * @param MaxDepth         Maximum depth (negative).
	 * @param MaxBurst         Maximum burst particle count.
	 * @return                 Burst count [0, MaxBurst].
	 */
	static int32 CalculateDebrisBurstCount(float SubductionDepth,
		float MaxDepth = DEFAULT_MAX_DEPTH,
		int32 MaxBurst = DEFAULT_MAX_DEBRIS_BURST);

	/**
	 * Map blight corruption ratio to smoke opacity.
	 *
	 * Clean world = invisible. Full blight = choking smog.
	 *
	 * @param BlightRatio  Global corruption [0, 1].
	 * @param MaxOpacity   Maximum smoke opacity.
	 * @return             Opacity [0, MaxOpacity].
	 */
	static float CalculateSmokeOpacity(float BlightRatio,
		float MaxOpacity = 1.0f);
};
