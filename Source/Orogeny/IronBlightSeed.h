// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellularBlightGrid.h"
#include "IronBlightSeed.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

/**
 * AIronBlightSeed
 *
 * Sprint 3: The Iron Blight — Systemic Parasitic Spread
 *
 * An invisible seed actor that drives cellular automata propagation.
 * The Blight does not think — it follows mathematical rules of spread.
 * It converts healthy Ecosystem flora to industrial ruin.
 *
 * ARCHITECTURE:
 *   - Internal FCellularBlightGrid handles all math (no UObject overhead).
 *   - Syncs to UDeepTimeSubsystem — expands once per in-game year.
 *   - Raycasts new corrupted cells onto the Titan mesh.
 *   - Spawns scaffolding HISM at hit locations.
 *   - Performs SphereOverlap to find and destroy Ecosystem flora.
 *
 * PERFORMANCE:
 *   - Cellular automata: O(n) where n = corrupted cells.
 *   - HISM: all scaffolding in a single Nanite draw call.
 *   - Expansion once per year (in Deep Time) — not per-frame.
 */
UCLASS()
class OROGENY_API AIronBlightSeed : public AActor
{
	GENERATED_BODY()

public:
	AIronBlightSeed();

	// -----------------------------------------------------------------------
	// Properties
	// -----------------------------------------------------------------------

	/** HISM for blight scaffolding visuals */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Blight")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> BlightScaffoldingHISM;

	/** World-space size of each grid cell (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Blight")
	float GridCellSize = 500.0f;

	/** Probability [0, 1] of infecting each neighbor per generation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Blight",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SpreadProbability = 0.25f;

	/** Radius for SphereOverlap to find and destroy ecosystem flora */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Blight")
	float TreeDestructionRadius = 600.0f;

	/** In-game days between expansion ticks (1 year) */
	static constexpr double EXPANSION_INTERVAL_DAYS = 365.25;

	/** Get the internal grid for inspection */
	const FCellularBlightGrid& GetGrid() const { return BlightGrid; }

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	/** Internal cellular automata grid */
	FCellularBlightGrid BlightGrid;

	/** Last in-game day an expansion was executed */
	double LastExpansionDay = 0.0;

	/** Execute one generation of blight spread */
	void ExpandBlight();
};
