// Copyright Orogeny. All Rights Reserved.

#include "NiagaraMath.h"

// ============================================================================
// CalculateDustSpawnRate — Velocity → Particle Rate
// ============================================================================
// PROOF:
//   Alpha=0.0 → 0.0 * 500 = 0       ✓ (Still)
//   Alpha=0.5 → 0.5 * 500 = 250     ✓ (Half speed)
//   Alpha=1.0 → 1.0 * 500 = 500     ✓ (Full sprint)
// ============================================================================

float FNiagaraMath::CalculateDustSpawnRate(
	float VelocityAlpha, float MaxSpawnRate)
{
	return FMath::Clamp(VelocityAlpha, 0.0f, 1.0f) * MaxSpawnRate;
}

// ============================================================================
// CalculateDebrisBurstCount — Depth → Burst Particles
// ============================================================================
// Both SubductionDepth and MaxDepth are negative.
// We use absolute values to normalize:
//   Depth=0       → 0/15000 = 0.0 → 0       ✓ (Surface)
//   Depth=-7500   → 7500/15000 = 0.5 → 500  ✓ (Half depth)
//   Depth=-15000  → 15000/15000 = 1.0 → 1000 ✓ (Max depth)
// ============================================================================

int32 FNiagaraMath::CalculateDebrisBurstCount(
	float SubductionDepth, float MaxDepth, int32 MaxBurst)
{
	const float AbsDepth = FMath::Abs(SubductionDepth);
	const float AbsMaxDepth = FMath::Abs(MaxDepth);

	if (AbsMaxDepth <= 0.0f)
	{
		return 0;
	}

	const float Ratio = FMath::Clamp(AbsDepth / AbsMaxDepth, 0.0f, 1.0f);
	return FMath::RoundToInt(Ratio * static_cast<float>(MaxBurst));
}

// ============================================================================
// CalculateSmokeOpacity — Blight → Smoke Density
// ============================================================================
// PROOF:
//   Blight=0.0 → 0.0 * 1.0 = 0.0   ✓ (Clean)
//   Blight=0.5 → 0.5 * 1.0 = 0.5   ✓ (Half)
//   Blight=1.0 → 1.0 * 1.0 = 1.0   ✓ (Choking)
// ============================================================================

float FNiagaraMath::CalculateSmokeOpacity(
	float BlightRatio, float MaxOpacity)
{
	return FMath::Clamp(BlightRatio, 0.0f, 1.0f) * MaxOpacity;
}
