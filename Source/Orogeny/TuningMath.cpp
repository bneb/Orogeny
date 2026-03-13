// Copyright Orogeny. All Rights Reserved.

#include "TuningMath.h"

// ============================================================================
// CalculateScaledBlightProbability — Difficulty → Blight Rate
// ============================================================================
// PROOF:
//   Base=0.20, Multi=2.0 → 0.40 → Clamp [0.01, 1.0] = 0.40  ✓
//   Base=0.80, Multi=2.0 → 1.60 → Clamp [0.01, 1.0] = 1.00  ✓ (capped)
//   Base=0.01, Multi=0.1 → 0.001 → Clamp [0.01, 1.0] = 0.01  ✓ (floor)
// ============================================================================

float FTuningMath::CalculateScaledBlightProbability(
	float BaseProbability, float DifficultyMultiplier)
{
	return FMath::Clamp(
		BaseProbability * DifficultyMultiplier,
		MIN_BLIGHT_PROBABILITY, 1.0f);
}

// ============================================================================
// CalculateScaledEcosystemHealing — Difficulty → Healing Rate
// ============================================================================
// PROOF:
//   Base=10.0, Multi=1.5 → 15.0  ✓ (easy mode)
//   Base=10.0, Multi=0.5 → 5.0   ✓ (hard mode)
// ============================================================================

float FTuningMath::CalculateScaledEcosystemHealing(
	float BaseHealing, float DifficultyMultiplier)
{
	return FMath::Clamp(
		BaseHealing * DifficultyMultiplier,
		0.0f, MAX_HEALING_RATE);
}

// ============================================================================
// IsEndlessMode — No Victory Gate
// ============================================================================
// PROOF:
//   Target=10.0 → false  ✓ (standard game)
//   Target=0.0  → true   ✓ (endless survivval)
//   Target=-1.0 → true   ✓ (defensive)
// ============================================================================

bool FTuningMath::IsEndlessMode(float TargetCenturies)
{
	return TargetCenturies <= 0.0f;
}
