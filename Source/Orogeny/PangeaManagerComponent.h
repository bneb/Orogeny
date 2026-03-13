// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PangeaManagerComponent.generated.h"

class ATerrainChunk;

/**
 * UPangeaManagerComponent
 *
 * Sprint 6: Infinite Terrain Streaming — The Continental Manager
 *
 * Tracks the player's chunk coordinate each tick. Spawns new chunks
 * within RenderDistance, destroys chunks beyond it. The world is
 * infinite — the player can walk forever and terrain generates
 * seamlessly in every direction.
 *
 * ARCHITECTURE:
 *   - TMap<FIntPoint, ATerrainChunk*> tracks all loaded chunks.
 *   - Each tick: compute player chunk, spawn missing, cull distant.
 *   - No NavMesh, no level streaming, no loading screens.
 *
 * MEMORY:
 *   - At RenderDistance=2: 5×5 = 25 chunks loaded max.
 *   - Each chunk ~2500 vertices = ~62,500 total vertices.
 *   - Destroyed chunks are fully garbage collected.
 */
UCLASS(ClassGroup = (Orogeny), meta = (BlueprintSpawnableComponent))
class OROGENY_API UPangeaManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPangeaManagerComponent();

	// -----------------------------------------------------------------------
	// Configuration
	// -----------------------------------------------------------------------

	/** Chunks loaded in each direction from the player (2 = 5×5 grid) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Terrain")
	int32 RenderDistance = 2;

	/** World-space size of each chunk edge (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Terrain")
	float ChunkSize = 10000.0f;

	/** Vertices per chunk edge */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Terrain")
	int32 GridResolution = 50;

	/** Noise frequency */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Terrain")
	float NoiseScale = 0.00005f;

	/** Height amplitude */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Terrain")
	float HeightMultiplier = 25000.0f;

	// -----------------------------------------------------------------------
	// State
	// -----------------------------------------------------------------------

	/** All currently loaded terrain chunks */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Terrain")
	TMap<FIntPoint, TObjectPtr<ATerrainChunk>> ActiveChunks;

	/** Last known player chunk coordinate (for delta check) */
	FIntPoint LastPlayerChunk = FIntPoint(INT32_MAX, INT32_MAX);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Spawn chunks within render distance */
	void SpawnMissingChunks(FIntPoint PlayerChunk);

	/** Destroy chunks beyond render distance */
	void CullDistantChunks(FIntPoint PlayerChunk);
};
