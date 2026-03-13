// Copyright Orogeny. All Rights Reserved.

#include "AtmosphericMath.h"

// ============================================================================
// EvaluateTimeOfDayAlpha — Sun Angle → Day/Night [0, 1]
// ============================================================================
// Absolute triangle wave:
//   Alpha = 1.0 - |( SunAngle - 180 ) / 180|
//
// PROOF:
//   0°   → 1.0 - |(-180)/180| = 1.0 - 1.0 = 0.0  ✓ (Midnight)
//   90°  → 1.0 - |(-90)/180|  = 1.0 - 0.5 = 0.5  ✓ (Dawn)
//   180° → 1.0 - |(0)/180|    = 1.0 - 0.0 = 1.0  ✓ (Noon)
//   270° → 1.0 - |(90)/180|   = 1.0 - 0.5 = 0.5  ✓ (Dusk)
//   360° → 1.0 - |(180)/180|  = 1.0 - 1.0 = 0.0  ✓ (Midnight)
// ============================================================================

float FAtmosphericMath::EvaluateTimeOfDayAlpha(float SunAngleDegrees)
{
	return 1.0f - FMath::Abs((SunAngleDegrees - 180.0f) / 180.0f);
}

// ============================================================================
// CalculateFogDensity — Blight Ratio → Fog Density
// ============================================================================
// Linear interpolation:
//   Density = BaseDensity * (1.0 + BlightRatio * (MaxMultiplier - 1.0))
//
// PROOF:
//   Blight=0.0: 0.02 * (1.0 + 0.0 * 4.0) = 0.02 * 1.0 = 0.02  ✓
//   Blight=1.0: 0.02 * (1.0 + 1.0 * 4.0) = 0.02 * 5.0 = 0.10  ✓
// ============================================================================

float FAtmosphericMath::CalculateFogDensity(
	float BaseDensity, float BlightRatio, float MaxBlightMultiplier)
{
	const float ClampedRatio = FMath::Clamp(BlightRatio, 0.0f, 1.0f);
	return BaseDensity * (1.0f + ClampedRatio * (MaxBlightMultiplier - 1.0f));
}

// ============================================================================
// BlendScatteringColor — Romantic Blue → Blight Amber
// ============================================================================

FLinearColor FAtmosphericMath::BlendScatteringColor(
	const FLinearColor& BaseColor,
	const FLinearColor& BlightColor,
	float BlightRatio)
{
	const float ClampedRatio = FMath::Clamp(BlightRatio, 0.0f, 1.0f);
	return FMath::Lerp(BaseColor, BlightColor, ClampedRatio);
}
