// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainChunk.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/**
 * ATerrainChunk
 *
 * Sprint 6: A Single Terrain Tile — Procedural Geology
 *
 * Each chunk is a GridResolution × GridResolution vertex grid
 * driven by deterministic Perlin noise. Chunks are spawned and
 * destroyed by UPangeaManagerComponent as the player moves.
 *
 * SEAMLESS: Heights are computed from absolute world coordinates,
 * not local offsets — chunk boundaries are mathematically flush.
 *
 * COLLISION: CreateMeshSection enables collision so that raycasts
 * from Ecosystem (tree placement) and Blight (scaffolding) hit
 * the terrain surface.
 */
UCLASS()
class OROGENY_API ATerrainChunk : public AActor
{
	GENERATED_BODY()

public:
	ATerrainChunk();

	// -----------------------------------------------------------------------
	// Components
	// -----------------------------------------------------------------------

	/** Procedural mesh — runtime generated terrain surface */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Terrain")
	TObjectPtr<UProceduralMeshComponent> ProcMesh;

	// -----------------------------------------------------------------------
	// Configuration
	// -----------------------------------------------------------------------

	/** Grid coordinate of this chunk */
	FIntPoint ChunkCoordinate;

	/** World-space size of each chunk edge (cm) */
	float ChunkSize = 10000.0f;

	/** Vertices per edge (50 = 2500 vertices per chunk) */
	int32 GridResolution = 50;

	/** Noise frequency */
	float NoiseScale = 0.00005f;

	/** Height amplitude */
	float HeightMultiplier = 25000.0f;

	// -----------------------------------------------------------------------
	// Material (Sprint 7)
	// -----------------------------------------------------------------------

	/** Base material to instance — must have BlightCorruption scalar param */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Material")
	TObjectPtr<UMaterialInterface> BaseTerrainMaterial;

	/** Per-chunk Dynamic Material Instance */
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Orogeny|Material")
	TObjectPtr<UMaterialInstanceDynamic> TerrainMID;

	// -----------------------------------------------------------------------
	// Generation
	// -----------------------------------------------------------------------

	/**
	 * Generate the terrain mesh from noise.
	 * Must be called after setting ChunkCoordinate and parameters.
	 */
	void GenerateMesh();

	/**
	 * Update per-chunk material parameters (e.g. blight corruption).
	 * Called by PangeaManager or Blight system.
	 *
	 * @param LocalBlightCorruption  [0, 1] corruption intensity for this chunk.
	 */
	void UpdateChunkMaterialData(float LocalBlightCorruption);
};
