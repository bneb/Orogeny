// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FUIMath
 *
 * Sprint 12: Pure UI Mathematics — The Player's Window
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * Every function is static, deterministic, and world-independent.
 *
 * ARCHITECTURE:
 *   - CalculateCurrentCentury: Days → 1-indexed century integer.
 *   - CalculateHealthPercentage: Normalized health → [0, 100] int.
 *   - CalculateBlightThreatLevel: Cell counts → [0, 1] ratio.
 *
 * DESIGN:
 *   All output formats are display-ready. No further string formatting
 *   needed in Blueprint — bind directly to Text Blocks.
 */
struct OROGENY_API FUIMath
{
	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Days per century (365.25 × 100) */
	static constexpr float DEFAULT_DAYS_PER_CENTURY = 36525.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Convert total elapsed days to a 1-indexed century number.
	 *
	 * Day 0 → Century 1. Day 36525 → Century 2.
	 *
	 * @param CurrentDay       Total elapsed days.
	 * @param DaysPerCentury   Days per century.
	 * @return                 Current century (1-indexed).
	 */
	static int32 CalculateCurrentCentury(float CurrentDay,
		float DaysPerCentury = DEFAULT_DAYS_PER_CENTURY);

	/**
	 * Convert normalized health to a display-ready integer [0, 100].
	 *
	 * @param EcosystemHealth  Current health.
	 * @param MaxHealth        Maximum health (normalization).
	 * @return                 Health percentage [0, 100].
	 */
	static int32 CalculateHealthPercentage(float EcosystemHealth,
		float MaxHealth = 1.0f);

	/**
	 * Calculate blight corruption ratio from cell counts.
	 *
	 * Safely handles TotalTrackedCells == 0 (returns 0.0).
	 *
	 * @param CorruptedCells      Number of corrupted cells.
	 * @param TotalTrackedCells   Total cells being tracked.
	 * @return                    Corruption ratio [0.0, 1.0].
	 */
	static float CalculateBlightThreatLevel(int32 CorruptedCells,
		int32 TotalTrackedCells);
};
