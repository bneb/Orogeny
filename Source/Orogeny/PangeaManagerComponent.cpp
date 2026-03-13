// Copyright Orogeny. All Rights Reserved.

#include "PangeaManagerComponent.h"
#include "Orogeny.h"
#include "TerrainChunk.h"
#include "ProceduralTerrainMath.h"
#include "Kismet/GameplayStatics.h"

// ============================================================================
// Constructor
// ============================================================================

UPangeaManagerComponent::UPangeaManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.25f; // 4Hz chunk check — not per-frame
}

// ============================================================================
// BeginPlay — Force initial chunk load
// ============================================================================

void UPangeaManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogOrogeny, Log,
		TEXT("UPangeaManagerComponent: Initialized. RenderDist=%d, ChunkSize=%.0f, Grid=%d"),
		RenderDistance, ChunkSize, GridResolution);

	// Force immediate spawn around origin
	SpawnMissingChunks(FIntPoint(0, 0));
}

// ============================================================================
// Tick — Track player chunk, spawn/cull
// ============================================================================

void UPangeaManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return;
	}

	const FIntPoint PlayerChunk = FProceduralTerrainMath::GetChunkFromWorldLocation(
		PlayerPawn->GetActorLocation(), ChunkSize);

	// Only update if the player moved to a new chunk
	if (PlayerChunk != LastPlayerChunk)
	{
		LastPlayerChunk = PlayerChunk;
		SpawnMissingChunks(PlayerChunk);
		CullDistantChunks(PlayerChunk);
	}
}

// ============================================================================
// SpawnMissingChunks — Fill the render distance grid
// ============================================================================

void UPangeaManagerComponent::SpawnMissingChunks(FIntPoint PlayerChunk)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	int32 SpawnedCount = 0;

	for (int32 DY = -RenderDistance; DY <= RenderDistance; ++DY)
	{
		for (int32 DX = -RenderDistance; DX <= RenderDistance; ++DX)
		{
			const FIntPoint ChunkCoord(PlayerChunk.X + DX, PlayerChunk.Y + DY);

			// Skip if already loaded
			if (ActiveChunks.Contains(ChunkCoord))
			{
				continue;
			}

			// Spawn at chunk's world-space origin
			const FVector SpawnLocation(
				ChunkCoord.X * ChunkSize,
				ChunkCoord.Y * ChunkSize,
				0.0f);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ATerrainChunk* NewChunk = World->SpawnActor<ATerrainChunk>(
				ATerrainChunk::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

			if (NewChunk)
			{
				NewChunk->ChunkCoordinate = ChunkCoord;
				NewChunk->ChunkSize = ChunkSize;
				NewChunk->GridResolution = GridResolution;
				NewChunk->NoiseScale = NoiseScale;
				NewChunk->HeightMultiplier = HeightMultiplier;
				NewChunk->GenerateMesh();

				ActiveChunks.Add(ChunkCoord, NewChunk);
				SpawnedCount++;
			}
		}
	}

	if (SpawnedCount > 0)
	{
		UE_LOG(LogOrogeny, Log,
			TEXT("Pangea: Spawned %d chunks around (%d, %d). Total active: %d"),
			SpawnedCount, PlayerChunk.X, PlayerChunk.Y, ActiveChunks.Num());
	}
}

// ============================================================================
// CullDistantChunks — Prevent memory leaks
// ============================================================================

void UPangeaManagerComponent::CullDistantChunks(FIntPoint PlayerChunk)
{
	const int32 CullDistance = RenderDistance + 1;
	TArray<FIntPoint> ChunksToCull;

	for (const auto& Pair : ActiveChunks)
	{
		const int32 DX = FMath::Abs(Pair.Key.X - PlayerChunk.X);
		const int32 DY = FMath::Abs(Pair.Key.Y - PlayerChunk.Y);

		if (DX > CullDistance || DY > CullDistance)
		{
			ChunksToCull.Add(Pair.Key);
		}
	}

	for (const FIntPoint& CullCoord : ChunksToCull)
	{
		if (ATerrainChunk* Chunk = ActiveChunks.FindRef(CullCoord))
		{
			Chunk->Destroy();
		}
		ActiveChunks.Remove(CullCoord);
	}

	if (ChunksToCull.Num() > 0)
	{
		UE_LOG(LogOrogeny, Log,
			TEXT("Pangea: Culled %d distant chunks. Active: %d"),
			ChunksToCull.Num(), ActiveChunks.Num());
	}
}
