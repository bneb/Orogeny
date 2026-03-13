// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FBiomeWeights
 *
 * Sprint 7: Biome layer blend weights for terrain materials.
 * Sum of all weights = 1.0 (normalized).
 * These weights drive the Triplanar material blend in the shader.
 */
struct OROGENY_API FBiomeWeights
{
	float GrassWeight = 0.0f;
	float RockWeight = 0.0f;
	float SnowWeight = 0.0f;
};

/**
 * FProceduralTerrainMath
 *
 * Sprint 6: Pure Terrain Mathematics — The Geology Engine
 * Sprint 7: Biome Blending Mathematics — Altitude × Slope
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * Every function is static, deterministic, and world-independent.
 *
 * ARCHITECTURE:
 *   - GetChunkFromWorldLocation: maps any world coordinate to a chunk grid.
 *     Uses FloorToInt for correct negative coordinate handling.
 *   - CalculateHeightAt: Perlin noise → height. Deterministic for a given
 *     (X, Y) — same input ALWAYS returns same output.
 *   - CalculateBiomeWeights: Altitude + Slope → Grass/Rock/Snow blend.
 *     Rock overrides Snow on steep faces (snow can't accumulate on cliffs).
 *
 * SEAMLESS GUARANTEE:
 *   The right edge of Chunk (0,0) and the left edge of Chunk (1,0)
 *   share identical world-space X coordinates, so PerlinNoise2D
 *   returns identical values. No stitching needed.
 */
struct OROGENY_API FProceduralTerrainMath
{
	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Default noise frequency (lower = broader terrain features) */
	static constexpr float DEFAULT_NOISE_SCALE = 0.00005f;

	/** Default height range in cm (±25000 = 500m total range) */
	static constexpr float DEFAULT_HEIGHT_MULTIPLIER = 25000.0f;

	/** Default chunk world size in cm (100m x 100m) */
	static constexpr float DEFAULT_CHUNK_SIZE = 10000.0f;

	/** Default snow line altitude in cm */
	static constexpr float DEFAULT_SNOW_LINE_Z = 25000.0f;

	/** Default rock slope threshold in degrees */
	static constexpr float DEFAULT_ROCK_SLOPE_ANGLE = 45.0f;

	/** Default biome blend sharpness (higher = harder transitions) */
	static constexpr float DEFAULT_BLEND_SHARPNESS = 5.0f;

	// Sprint 8.5: FBM Constants
	/** FBM octave count (more = finer detail, more cost) */
	static constexpr int32 DEFAULT_FBM_OCTAVES = 6;

	/** Persistence: amplitude decay per octave (0.5 = halves each level) */
	static constexpr float DEFAULT_PERSISTENCE = 0.5f;

	/** Lacunarity: frequency growth per octave (2.0 = doubles each level) */
	static constexpr float DEFAULT_LACUNARITY = 2.0f;

	/** Continental shelf height (FBM base layer, cm) */
	static constexpr float FBM_HEIGHT_MULTIPLIER = 15000.0f;

	/** Alpine ridge height (ridged overlay, cm) */
	static constexpr float RIDGED_HEIGHT_MULTIPLIER = 25000.0f;

	/** Altitude threshold for alpine blending (cm) */
	static constexpr float ALPINE_BLEND_THRESHOLD = 5000.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Convert a world-space location to chunk grid coordinates.
	 */
	static FIntPoint GetChunkFromWorldLocation(const FVector& WorldLoc, float ChunkSize);

	/**
	 * Fractional Brownian Motion — layered Perlin noise.
	 *
	 * Sprint 8.5: Standard FBM loop. Each octave adds finer detail
	 * with decreasing amplitude (Persistence) and increasing frequency
	 * (Lacunarity). Result is normalized by the amplitude sum.
	 *
	 * @param X, Y         World coordinates.
	 * @param Octaves      Number of noise layers.
	 * @param Persistence  Amplitude decay per octave.
	 * @param Lacunarity   Frequency growth per octave.
	 * @param Scale        Base frequency.
	 * @return             Noise value (approximately [-1, 1]).
	 */
	static float CalculateFBM(float X, float Y, int32 Octaves,
		float Persistence, float Lacunarity, float Scale);

	/**
	 * Ridged Multifractal Noise — sharp geological ridges.
	 *
	 * Sprint 8.5: Inverts and squares the Perlin output to create
	 * sharp ridgelines. Result is always >= 0.
	 *
	 * @param X, Y         World coordinates.
	 * @param Octaves      Number of noise layers.
	 * @param Persistence  Amplitude decay per octave.
	 * @param Lacunarity   Frequency growth per octave.
	 * @param Scale        Base frequency.
	 * @return             Noise value (always >= 0).
	 */
	static float CalculateRidged(float X, float Y, int32 Octaves,
		float Persistence, float Lacunarity, float Scale);

	/**
	 * Calculate terrain height at a world-space coordinate.
	 *
	 * Sprint 8.5 REFACTOR: Composite terrain generation.
	 *   Layer 1: FBM continental shelf (broad rolling hills).
	 *   Layer 2: Ridged alpine piercers (sharp mountain ridges).
	 *   Blend: BaseHeight masks AlpineHeight above ALPINE_BLEND_THRESHOLD.
	 *
	 * DETERMINISTIC: Same (X, Y) ALWAYS returns the same height.
	 * SEAMLESS: World-space coordinates → no chunk boundary artifacts.
	 *
	 * @param WorldX           World X coordinate.
	 * @param WorldY           World Y coordinate.
	 * @param NoiseScale       Base frequency multiplier.
	 * @param HeightMultiplier Legacy parameter (used as overall scale).
	 * @return                 Z height value.
	 */
	static float CalculateHeightAt(float WorldX, float WorldY,
		float NoiseScale, float HeightMultiplier);

	/**
	 * Calculate biome layer weights from altitude and surface normal.
	 */
	static FBiomeWeights CalculateBiomeWeights(
		float ZHeight, const FVector& SurfaceNormal,
		float SnowLineZ = DEFAULT_SNOW_LINE_Z,
		float RockSlopeAngle = DEFAULT_ROCK_SLOPE_ANGLE,
		float BlendSharpness = DEFAULT_BLEND_SHARPNESS);
};
