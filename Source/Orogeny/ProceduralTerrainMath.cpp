// Copyright Orogeny. All Rights Reserved.

#include "ProceduralTerrainMath.h"

// ============================================================================
// GetChunkFromWorldLocation — FloorToInt for correct negative mapping
// ============================================================================
// PROOF:
//   (15000, 5000) / 10000 → (1.5, 0.5) → Floor → (1, 0)  ✓
//   (-5000, -15000) / 10000 → (-0.5, -1.5) → Floor → (-1, -2)  ✓
//   (0, 0) / 10000 → (0, 0) → Floor → (0, 0)  ✓
// ============================================================================

FIntPoint FProceduralTerrainMath::GetChunkFromWorldLocation(
	const FVector& WorldLoc, float ChunkSize)
{
	return FIntPoint(
		FMath::FloorToInt(WorldLoc.X / ChunkSize),
		FMath::FloorToInt(WorldLoc.Y / ChunkSize));
}

// ============================================================================
// CalculateFBM — Fractional Brownian Motion
// ============================================================================
// Standard layered Perlin noise. Each octave adds finer detail:
//   - Amplitude shrinks by Persistence (typically 0.5)
//   - Frequency grows by Lacunarity (typically 2.0)
//
// Result is normalized by the total amplitude sum to keep output
// approximately in [-1, 1].
// ============================================================================

float FProceduralTerrainMath::CalculateFBM(
	float X, float Y, int32 Octaves,
	float Persistence, float Lacunarity, float Scale)
{
	float Total = 0.0f;
	float Amplitude = 1.0f;
	float Frequency = Scale;
	float MaxAmplitude = 0.0f;

	for (int32 i = 0; i < Octaves; ++i)
	{
		Total += FMath::PerlinNoise2D(
			FVector2D(X * Frequency, Y * Frequency)) * Amplitude;
		MaxAmplitude += Amplitude;
		Amplitude *= Persistence;
		Frequency *= Lacunarity;
	}

	// Normalize to approximately [-1, 1]
	return (MaxAmplitude > 0.0f) ? (Total / MaxAmplitude) : 0.0f;
}

// ============================================================================
// CalculateRidged — Ridged Multifractal Noise
// ============================================================================
// Invert and square the Perlin output for sharp geological ridges:
//   NoiseVal = 1.0 - |PerlinNoise(...)|   → invert
//   NoiseVal *= NoiseVal                   → square for sharper peaks
//
// GUARANTEE: Output is ALWAYS >= 0 (due to Abs + square).
// ============================================================================

float FProceduralTerrainMath::CalculateRidged(
	float X, float Y, int32 Octaves,
	float Persistence, float Lacunarity, float Scale)
{
	float Total = 0.0f;
	float Amplitude = 1.0f;
	float Frequency = Scale;
	float MaxAmplitude = 0.0f;

	for (int32 i = 0; i < Octaves; ++i)
	{
		float NoiseVal = FMath::PerlinNoise2D(
			FVector2D(X * Frequency, Y * Frequency));
		NoiseVal = 1.0f - FMath::Abs(NoiseVal);   // Invert
		NoiseVal *= NoiseVal;                       // Square for ridges

		Total += NoiseVal * Amplitude;
		MaxAmplitude += Amplitude;
		Amplitude *= Persistence;
		Frequency *= Lacunarity;
	}

	return (MaxAmplitude > 0.0f) ? (Total / MaxAmplitude) : 0.0f;
}

// ============================================================================
// CalculateHeightAt — Composite Terrain (Sprint 8.5 Refactor)
// ============================================================================
// Two-layer composite:
//   Layer 1: FBM continental shelf → broad, rolling terrain.
//   Layer 2: Ridged alpine piercers → sharp mountain ridges.
//
// BLEND:
//   Alpine ridges fade in above ALPINE_BLEND_THRESHOLD using the
//   FBM base height as a mask. Low valleys stay smooth; high
//   plateaus erupt into jagged peaks.
//
// BACKWARD COMPATIBLE:
//   Same (WorldX, WorldY) always returns the same height.
//   Same function signature — chunk manager unchanged.
// ============================================================================

float FProceduralTerrainMath::CalculateHeightAt(
	float WorldX, float WorldY,
	float NoiseScale, float HeightMultiplier)
{
	// Layer 1: Continental shelf (smooth rolling terrain)
	const float BaseHeight = CalculateFBM(
		WorldX, WorldY,
		DEFAULT_FBM_OCTAVES, DEFAULT_PERSISTENCE, DEFAULT_LACUNARITY,
		NoiseScale) * FBM_HEIGHT_MULTIPLIER;

	// Layer 2: Alpine ridges (sharp mountain peaks)
	// Use a slightly different frequency to avoid correlation
	const float AlpineNoise = CalculateRidged(
		WorldX, WorldY,
		DEFAULT_FBM_OCTAVES, DEFAULT_PERSISTENCE, DEFAULT_LACUNARITY,
		NoiseScale * 1.7f);
	const float AlpineHeight = AlpineNoise * RIDGED_HEIGHT_MULTIPLIER;

	// Blend: Alpine fades in above the threshold
	const float AlphaBlend = FMath::Clamp(
		(BaseHeight - ALPINE_BLEND_THRESHOLD) /
		(FBM_HEIGHT_MULTIPLIER - ALPINE_BLEND_THRESHOLD),
		0.0f, 1.0f);

	return BaseHeight + (AlpineHeight * AlphaBlend);
}

// ============================================================================
// CalculateBiomeWeights — Altitude × Slope → Grass/Rock/Snow
// ============================================================================
// PROOF:
//   Flat valley (Z=5000, Normal=Up):    Grass=1, Rock=0, Snow=0  ✓
//   Steep cliff (Z=5000, Normal=Right): Rock=1,  Grass=0, Snow=0 ✓
//   High flat (Z=30000, Normal=Up):     Snow=1,  Grass=0, Rock=0 ✓
//   High cliff (Z=30000, Normal=Right): Rock=1,  Snow=0,  Grass=0 ✓
//     (Snow cannot accumulate on vertical faces → rock override)
// ============================================================================

FBiomeWeights FProceduralTerrainMath::CalculateBiomeWeights(
	float ZHeight, const FVector& SurfaceNormal,
	float SnowLineZ, float RockSlopeAngle, float BlendSharpness)
{
	FBiomeWeights Weights;

	// 1. Calculate slope angle from surface normal vs. world UP
	const float DotUp = FMath::Clamp(FVector::DotProduct(SurfaceNormal.GetSafeNormal(),
		FVector::UpVector), 0.0f, 1.0f);
	const float SlopeAngleDeg = FMath::RadiansToDegrees(FMath::Acos(DotUp));

	// 2. Rock weight: steep slopes → rock
	//    Saturate the blend so slopes well beyond threshold = full rock
	const float RockAlpha = FMath::Clamp(
		(SlopeAngleDeg - RockSlopeAngle) / (90.0f - RockSlopeAngle) * BlendSharpness,
		0.0f, 1.0f);
	Weights.RockWeight = RockAlpha;

	// 3. Snow weight: high altitude → snow
	const float SnowAlpha = FMath::Clamp(
		(ZHeight - SnowLineZ) / (SnowLineZ * 0.2f) * BlendSharpness,
		0.0f, 1.0f);

	// 4. Rock override: snow cannot accumulate on sheer cliffs
	Weights.SnowWeight = SnowAlpha * (1.0f - Weights.RockWeight);

	// 5. Grass is the remainder
	Weights.GrassWeight = FMath::Clamp(1.0f - Weights.RockWeight - Weights.SnowWeight,
		0.0f, 1.0f);

	return Weights;
}

