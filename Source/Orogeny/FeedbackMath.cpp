// Copyright Orogeny. All Rights Reserved.

#include "FeedbackMath.h"

// ============================================================================
// CalculateFootstepShakeScale — Velocity → Shake Intensity
// ============================================================================
// PROOF:
//   Alpha=0.0:  < MinSpeedAlpha(0.1) → return 0.0    ✓ (Still)
//   Alpha=0.05: < MinSpeedAlpha(0.1) → return 0.0    ✓ (Creeping)
//   Alpha=0.1:  at threshold → (0.1-0.1)/(0.9)*0.9+0.1 = 0.1  ✓
//   Alpha=1.0:  (1.0-0.1)/(0.9)*0.9+0.1 = 1.0       ✓ (Full sprint)
// ============================================================================

float FFeedbackMath::CalculateFootstepShakeScale(
	float VelocityAlpha, float MinSpeedAlpha)
{
	if (VelocityAlpha < MinSpeedAlpha)
	{
		return 0.0f;
	}

	// Map [MinSpeedAlpha, 1.0] → [0.1, 1.0]
	const float NormalizedAlpha = (VelocityAlpha - MinSpeedAlpha) / (1.0f - MinSpeedAlpha);
	return FMath::Clamp(NormalizedAlpha * 0.9f + 0.1f, 0.0f, 1.0f);
}

// ============================================================================
// CalculateHealthVignetteWeight — Health → Vignette Darkness
// ============================================================================
// PROOF:
//   Health=1.0:  1.0/1.0 = 1.0 > 0.3 → return 0.0   ✓ (Healthy)
//   Health=0.15: 0.15/1.0 = 0.15 < 0.3 → (0.3-0.15)/0.3 = 0.5  ✓ (Critical)
//   Health=0.0:  0.0/1.0 = 0.0 < 0.3 → (0.3-0.0)/0.3 = 1.0     ✓ (Dead)
// ============================================================================

float FFeedbackMath::CalculateHealthVignetteWeight(
	float EcosystemHealth, float MaxHealth, float CriticalThreshold)
{
	const float HealthRatio = (MaxHealth > 0.0f) ?
		FMath::Clamp(EcosystemHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;

	if (HealthRatio >= CriticalThreshold)
	{
		return 0.0f;
	}

	// Map [CriticalThreshold, 0.0] → [0.0, 1.0]
	return FMath::Clamp((CriticalThreshold - HealthRatio) / CriticalThreshold, 0.0f, 1.0f);
}

// ============================================================================
// CalculateSubductionRumbleIntensity — Depth → Rumble
// ============================================================================
// PROOF:
//   Not transitioning → return 0.0                    ✓ (Idle)
//   Transitioning, depth=0     → 0.5 (min rumble)    ✓
//   Transitioning, depth=max   → 1.0 (full grind)    ✓
// ============================================================================

float FFeedbackMath::CalculateSubductionRumbleIntensity(
	bool bIsTransitioning, float CurrentDepth, float MaxDepth)
{
	if (!bIsTransitioning)
	{
		return 0.0f;
	}

	// Both CurrentDepth and MaxDepth are negative (sinking down)
	// Map CurrentDepth/MaxDepth → [0, 1] then scale to [0.5, 1.0]
	const float DepthRatio = (MaxDepth != 0.0f) ?
		FMath::Clamp(CurrentDepth / MaxDepth, 0.0f, 1.0f) : 0.0f;

	return FMath::Lerp(0.5f, 1.0f, DepthRatio);
}
