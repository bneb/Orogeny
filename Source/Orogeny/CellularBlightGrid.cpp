// Copyright Orogeny. All Rights Reserved.

#include "CellularBlightGrid.h"

// ============================================================================
// GetNeighbors — Von Neumann Neighborhood (4 cardinals)
// ============================================================================
// No diagonals. The Blight spreads like infrastructure:
// roads, railways, pipelines — always orthogonal.
// ============================================================================

TArray<FIntPoint> FCellularBlightGrid::GetNeighbors(FIntPoint Cell)
{
	TArray<FIntPoint> Neighbors;
	Neighbors.Reserve(4);

	Neighbors.Add(FIntPoint(Cell.X,     Cell.Y + 1)); // North
	Neighbors.Add(FIntPoint(Cell.X,     Cell.Y - 1)); // South
	Neighbors.Add(FIntPoint(Cell.X + 1, Cell.Y));     // East
	Neighbors.Add(FIntPoint(Cell.X - 1, Cell.Y));     // West

	return Neighbors;
}

// ============================================================================
// CalculateNextGeneration — Snapshot-based Propagation
// ============================================================================
// CRITICAL: We snapshot the current corrupted set BEFORE mutating.
// This prevents cascading infections within a single generation.
// A cell infected this tick cannot spread until the NEXT tick.
// ============================================================================

void FCellularBlightGrid::CalculateNextGeneration(
	TMap<FIntPoint, bool>& InOutGrid, float SpreadProbability)
{
	const float ClampedProbability = FMath::Clamp(SpreadProbability, 0.0f, 1.0f);

	// Snapshot: collect all currently corrupted cells
	TArray<FIntPoint> CurrentCorrupted;
	for (const auto& Pair : InOutGrid)
	{
		if (Pair.Value)
		{
			CurrentCorrupted.Add(Pair.Key);
		}
	}

	// Evaluate neighbors of each corrupted cell
	for (const FIntPoint& Cell : CurrentCorrupted)
	{
		TArray<FIntPoint> Neighbors = GetNeighbors(Cell);

		for (const FIntPoint& Neighbor : Neighbors)
		{
			// Skip if already corrupted
			const bool* Existing = InOutGrid.Find(Neighbor);
			if (Existing && *Existing)
			{
				continue;
			}

			// Probabilistic spread
			if (FMath::FRand() <= ClampedProbability)
			{
				InOutGrid.Add(Neighbor, true);
			}
		}
	}
}

// ============================================================================
// Utility
// ============================================================================

int32 FCellularBlightGrid::CountCorrupted(const TMap<FIntPoint, bool>& InGrid)
{
	int32 Count = 0;
	for (const auto& Pair : InGrid)
	{
		if (Pair.Value)
		{
			Count++;
		}
	}
	return Count;
}

void FCellularBlightGrid::SeedAt(FIntPoint Location)
{
	Grid.Add(Location, true);
}

TArray<FIntPoint> FCellularBlightGrid::GetCorruptedCells() const
{
	TArray<FIntPoint> Result;
	for (const auto& Pair : Grid)
	{
		if (Pair.Value)
		{
			Result.Add(Pair.Key);
		}
	}
	return Result;
}
