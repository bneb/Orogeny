// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * FCellularBlightGrid
 *
 * Sprint 3: Pure C++ Cellular Automata — The Iron Blight's Mind
 *
 * NOT a UObject — pure math, zero engine overhead, fully TDD-testable.
 * The Blight does not think. It does not pathfind. It SPREADS.
 * Like rust, like fungus, like industry.
 *
 * ARCHITECTURE:
 *   - TMap<FIntPoint, bool> grid: true = corrupted, absent/false = clean
 *   - GetNeighbors: returns 4 cardinal adjacents (Von Neumann neighborhood)
 *   - CalculateNextGeneration: for each corrupted cell, probabilistically
 *     infects uncorrupted neighbors. Single-pass with copy to prevent
 *     read-during-write corruption.
 *
 * PERFORMANCE:
 *   - TMap lookup is O(1) amortized.
 *   - Only processes corrupted cells (sparse iteration).
 *   - No NavMesh, no A*, no pathfinding overhead.
 */
struct OROGENY_API FCellularBlightGrid
{
	/** The grid state. Key = cell coordinate, Value = true if corrupted. */
	TMap<FIntPoint, bool> Grid;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Get the 4 cardinal neighbors of a cell (Von Neumann neighborhood).
	 * North (+Y), South (-Y), East (+X), West (-X).
	 *
	 * @param Cell  The grid coordinate to query.
	 * @return      Array of 4 adjacent coordinates.
	 */
	static TArray<FIntPoint> GetNeighbors(FIntPoint Cell);

	/**
	 * Calculate the next generation of blight spread.
	 *
	 * For every corrupted cell, each uncorrupted neighbor has a
	 * SpreadProbability chance of becoming corrupted.
	 *
	 * Uses a snapshot copy to prevent read-during-write corruption:
	 * new infections in this generation don't cascade within the same tick.
	 *
	 * @param InOutGrid          The grid to mutate in-place.
	 * @param SpreadProbability  Probability [0, 1] of infecting each neighbor.
	 */
	static void CalculateNextGeneration(TMap<FIntPoint, bool>& InOutGrid, float SpreadProbability);

	// -----------------------------------------------------------------------
	// Utility
	// -----------------------------------------------------------------------

	/** Count the number of corrupted cells in the grid. */
	static int32 CountCorrupted(const TMap<FIntPoint, bool>& InGrid);

	/** Seed the grid with an initial corrupted cell. */
	void SeedAt(FIntPoint Location);

	/** Get all corrupted cell coordinates. */
	TArray<FIntPoint> GetCorruptedCells() const;
};
