// Copyright Orogeny. All Rights Reserved.

#include "UIMath.h"

// ============================================================================
// CalculateCurrentCentury — Days → 1-Indexed Century
// ============================================================================
// PROOF:
//   Day 0       → Floor(0 / 36525) + 1 = 0 + 1 = 1     ✓
//   Day 36524.9 → Floor(36524.9 / 36525) + 1 = 0 + 1 = 1 ✓
//   Day 36525.0 → Floor(36525 / 36525) + 1 = 1 + 1 = 2   ✓
// ============================================================================

int32 FUIMath::CalculateCurrentCentury(float CurrentDay, float DaysPerCentury)
{
	return FMath::FloorToInt(CurrentDay / DaysPerCentury) + 1;
}

// ============================================================================
// CalculateHealthPercentage — [0, 1] → [0, 100] Integer
// ============================================================================
// PROOF:
//   Health=0.456 → 0.456/1.0 = 0.456 → Floor(45.6) = 45  ✓
//   Health=1.0   → 1.0/1.0 = 1.0 → Floor(100) = 100      ✓
//   Health=0.0   → 0.0/1.0 = 0.0 → Floor(0) = 0          ✓
// ============================================================================

int32 FUIMath::CalculateHealthPercentage(float EcosystemHealth, float MaxHealth)
{
	const float Ratio = (MaxHealth > 0.0f) ?
		FMath::Clamp(EcosystemHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;
	return FMath::FloorToInt(Ratio * 100.0f);
}

// ============================================================================
// CalculateBlightThreatLevel — Safe Division
// ============================================================================
// PROOF:
//   Corrupted=10, Total=0   → return 0.0 (no crash)     ✓
//   Corrupted=25, Total=100 → 25/100 = 0.25             ✓
//   Corrupted=200, Total=100 → Clamp → 1.0              ✓
// ============================================================================

float FUIMath::CalculateBlightThreatLevel(
	int32 CorruptedCells, int32 TotalTrackedCells)
{
	if (TotalTrackedCells <= 0)
	{
		return 0.0f;
	}

	return FMath::Clamp(
		static_cast<float>(CorruptedCells) / static_cast<float>(TotalTrackedCells),
		0.0f, 1.0f);
}
