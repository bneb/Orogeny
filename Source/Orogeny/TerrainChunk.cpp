// Copyright Orogeny. All Rights Reserved.

#include "TerrainChunk.h"
#include "Orogeny.h"
#include "ProceduralTerrainMath.h"
#include "ProceduralMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// ============================================================================
// Constructor
// ============================================================================

ATerrainChunk::ATerrainChunk()
{
	PrimaryActorTick.bCanEverTick = false; // Static once generated

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMesh"));
	RootComponent = ProcMesh;

	// Enable collision for raycasts (Ecosystem, Blight)
	ProcMesh->bUseComplexAsSimpleCollision = true;
	ProcMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProcMesh->SetCollisionResponseToAllChannels(ECR_Block);
}

// ============================================================================
// GenerateMesh — Build terrain from Perlin noise
// ============================================================================

void ATerrainChunk::GenerateMesh()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FLinearColor> VertexColors;
	TArray<FProcMeshTangent> Tangents;

	const int32 NumVerts = GridResolution + 1;
	const float CellSize = ChunkSize / static_cast<float>(GridResolution);

	// World-space origin of this chunk
	const float ChunkWorldX = ChunkCoordinate.X * ChunkSize;
	const float ChunkWorldY = ChunkCoordinate.Y * ChunkSize;

	// -----------------------------------------------------------------------
	// Generate vertices
	// -----------------------------------------------------------------------
	Vertices.Reserve(NumVerts * NumVerts);
	Normals.Reserve(NumVerts * NumVerts);
	UV0.Reserve(NumVerts * NumVerts);

	for (int32 Y = 0; Y < NumVerts; ++Y)
	{
		for (int32 X = 0; X < NumVerts; ++X)
		{
			// Absolute world coordinates for noise sampling
			const float WorldX = ChunkWorldX + X * CellSize;
			const float WorldY = ChunkWorldY + Y * CellSize;

			// Height from deterministic noise
			const float Height = FProceduralTerrainMath::CalculateHeightAt(
				WorldX, WorldY, NoiseScale, HeightMultiplier);

			// Vertex in local space (relative to chunk actor)
			Vertices.Add(FVector(X * CellSize, Y * CellSize, Height));

			// Default normal (will be recomputed below)
			Normals.Add(FVector(0.0f, 0.0f, 1.0f));

			// UV: normalized [0, 1] across the chunk
			UV0.Add(FVector2D(
				static_cast<float>(X) / static_cast<float>(GridResolution),
				static_cast<float>(Y) / static_cast<float>(GridResolution)));
		}
	}

	// -----------------------------------------------------------------------
	// Generate triangles (two per quad, CCW winding)
	// -----------------------------------------------------------------------
	Triangles.Reserve(GridResolution * GridResolution * 6);

	for (int32 Y = 0; Y < GridResolution; ++Y)
	{
		for (int32 X = 0; X < GridResolution; ++X)
		{
			const int32 BL = Y * NumVerts + X;         // Bottom-left
			const int32 BR = BL + 1;                   // Bottom-right
			const int32 TL = (Y + 1) * NumVerts + X;   // Top-left
			const int32 TR = TL + 1;                   // Top-right

			// First triangle (BL → TL → BR)
			Triangles.Add(BL);
			Triangles.Add(TL);
			Triangles.Add(BR);

			// Second triangle (BR → TL → TR)
			Triangles.Add(BR);
			Triangles.Add(TL);
			Triangles.Add(TR);
		}
	}

	// -----------------------------------------------------------------------
	// Recompute normals from triangle adjacency
	// -----------------------------------------------------------------------
	for (int32 i = 0; i < Normals.Num(); ++i)
	{
		Normals[i] = FVector::ZeroVector;
	}

	for (int32 i = 0; i < Triangles.Num(); i += 3)
	{
		const FVector& V0 = Vertices[Triangles[i]];
		const FVector& V1 = Vertices[Triangles[i + 1]];
		const FVector& V2 = Vertices[Triangles[i + 2]];

		const FVector FaceNormal = FVector::CrossProduct(V1 - V0, V2 - V0).GetSafeNormal();

		Normals[Triangles[i]] += FaceNormal;
		Normals[Triangles[i + 1]] += FaceNormal;
		Normals[Triangles[i + 2]] += FaceNormal;
	}

	for (int32 i = 0; i < Normals.Num(); ++i)
	{
		Normals[i] = Normals[i].GetSafeNormal();
	}

	// -----------------------------------------------------------------------
	// Create the mesh section
	// -----------------------------------------------------------------------
	ProcMesh->CreateMeshSection_LinearColor(
		0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);

	// -----------------------------------------------------------------------
	// Sprint 7: Create Dynamic Material Instance
	// -----------------------------------------------------------------------
	if (BaseTerrainMaterial)
	{
		TerrainMID = UMaterialInstanceDynamic::Create(BaseTerrainMaterial, this);
		ProcMesh->SetMaterial(0, TerrainMID);

		UE_LOG(LogOrogeny, Log,
			TEXT("ATerrainChunk (%d, %d): MID created from %s"),
			ChunkCoordinate.X, ChunkCoordinate.Y,
			*BaseTerrainMaterial->GetName());
	}

	UE_LOG(LogOrogeny, Log,
		TEXT("ATerrainChunk (%d, %d): Generated %d vertices, %d triangles."),
		ChunkCoordinate.X, ChunkCoordinate.Y,
		Vertices.Num(), Triangles.Num() / 3);
}

// ============================================================================
// UpdateChunkMaterialData — Per-chunk shader parameters
// ============================================================================

void ATerrainChunk::UpdateChunkMaterialData(float LocalBlightCorruption)
{
	if (TerrainMID)
	{
		TerrainMID->SetScalarParameterValue(FName("BlightCorruption"), LocalBlightCorruption);
	}
}

