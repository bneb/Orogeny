// Copyright Orogeny. All Rights Reserved.

#include "MetaSoundMath.h"

// ============================================================================
// CalculateWindIntensity — Altitude → Wind [0, 1]
// ============================================================================
// PROOF:
//   Z=0     → (0 - 5000) / 20000 = -0.25 → Clamp → 0.0  ✓ (Valley)
//   Z=15000 → (15000 - 5000) / 20000 = 0.5 → 0.5         ✓ (Midpoint)
//   Z=30000 → (30000 - 5000) / 20000 = 1.25 → Clamp → 1.0 ✓ (Peak)
// ============================================================================

float FMetaSoundMath::CalculateWindIntensity(
	float ZHeight, float ValleyZ, float PeakZ)
{
	return FMath::Clamp((ZHeight - ValleyZ) / (PeakZ - ValleyZ), 0.0f, 1.0f);
}

// ============================================================================
// CalculateFaunaVolume — Health → Chorus Volume [0, 1]
// ============================================================================
// Linear pass-through. MetaSound graph applies easing.
// PROOF:
//   Health=0.0 → 0.0 / 1.0 = 0.0  ✓ (Silent)
//   Health=1.0 → 1.0 / 1.0 = 1.0  ✓ (Full chorus)
// ============================================================================

float FMetaSoundMath::CalculateFaunaVolume(
	float EcosystemHealth, float MaxHealth)
{
	return FMath::Clamp(EcosystemHealth / MaxHealth, 0.0f, 1.0f);
}

// ============================================================================
// CalculateTimeCompressionPitch — Log Compression [1.0, MaxPitch]
// ============================================================================
// Deep Time can accelerate to 36,525x. Linear pitch would be insane.
// Logarithmic compression:
//   LogScale = ln(CurrentScale) / ln(MaxScale)  → [0, 1]
//   Pitch = Lerp(1.0, MaxPitch, LogScale)
//
// PROOF:
//   Scale=1.0   → <= BaseScale → return 1.0  ✓ (No pitch shift)
//   Scale=36525 → ln(36525)/ln(36525) = 1.0 → Lerp(1, 3, 1) = 3.0  ✓
// ============================================================================

float FMetaSoundMath::CalculateTimeCompressionPitch(
	float CurrentTimeScale, float BaseScale, float MaxScale, float MaxPitch)
{
	if (CurrentTimeScale <= BaseScale)
	{
		return 1.0f;
	}

	const float ClampedScale = FMath::Clamp(CurrentTimeScale, BaseScale, MaxScale);
	const float LogScale = FMath::Loge(ClampedScale) / FMath::Loge(MaxScale);
	return FMath::Lerp(1.0f, MaxPitch, LogScale);
}
