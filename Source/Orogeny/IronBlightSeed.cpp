// Copyright Orogeny. All Rights Reserved.

#include "IronBlightSeed.h"
#include "Orogeny.h"
#include "DeepTimeSubsystem.h"
#include "EcosystemArmorComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// ============================================================================
// Constructor
// ============================================================================

AIronBlightSeed::AIronBlightSeed()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f; // 10Hz — expansion is yearly

	// Scaffolding HISM — root component
	BlightScaffoldingHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(
		TEXT("BlightScaffoldingHISM"));
	RootComponent = BlightScaffoldingHISM;
	BlightScaffoldingHISM->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BlightScaffoldingHISM->SetCastShadow(true);
}

// ============================================================================
// BeginPlay — Seed the initial corruption
// ============================================================================

void AIronBlightSeed::BeginPlay()
{
	Super::BeginPlay();

	// Seed at origin (0, 0) in grid space
	BlightGrid.SeedAt(FIntPoint(0, 0));

	UE_LOG(LogOrogeny, Log,
		TEXT("AIronBlightSeed: Seeded at grid (0,0). CellSize=%.0f, SpreadProb=%.2f"),
		GridCellSize, SpreadProbability);
}

// ============================================================================
// Tick — Sync to Deep Time for yearly expansion
// ============================================================================

void AIronBlightSeed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UDeepTimeSubsystem* DeepTime = World->GetSubsystem<UDeepTimeSubsystem>();
	if (!DeepTime)
	{
		return;
	}

	const double CurrentDay = DeepTime->CurrentDay;
	if (CurrentDay - LastExpansionDay >= EXPANSION_INTERVAL_DAYS)
	{
		LastExpansionDay = CurrentDay;
		ExpandBlight();
	}
}

// ============================================================================
// ExpandBlight — One CA generation + world placement + ecosystem damage
// ============================================================================

void AIronBlightSeed::ExpandBlight()
{
	// Snapshot pre-expansion cells to know which are NEW after generation
	TArray<FIntPoint> PreExpansionCells = BlightGrid.GetCorruptedCells();
	TSet<FIntPoint> PreExpansionSet;
	for (const FIntPoint& Cell : PreExpansionCells)
	{
		PreExpansionSet.Add(Cell);
	}

	// Run one generation
	FCellularBlightGrid::CalculateNextGeneration(BlightGrid.Grid, SpreadProbability);

	// Find newly corrupted cells
	TArray<FIntPoint> PostExpansionCells = BlightGrid.GetCorruptedCells();
	TArray<FIntPoint> NewCells;
	for (const FIntPoint& Cell : PostExpansionCells)
	{
		if (!PreExpansionSet.Contains(Cell))
		{
			NewCells.Add(Cell);
		}
	}

	if (NewCells.Num() == 0)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || !BlightScaffoldingHISM || !BlightScaffoldingHISM->GetStaticMesh())
	{
		return;
	}

	// Convert grid coordinates to world space and raycast
	const FVector SeedLocation = GetActorLocation();
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	int32 PlacedCount = 0;
	int32 TreesDestroyed = 0;

	for (const FIntPoint& Cell : NewCells)
	{
		const FVector WorldXY = SeedLocation + FVector(
			Cell.X * GridCellSize,
			Cell.Y * GridCellSize,
			0.0f);

		const FVector RayStart = WorldXY + FVector(0.0f, 0.0f, 50000.0f);
		const FVector RayEnd = WorldXY - FVector(0.0f, 0.0f, 50000.0f);

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, RayStart, RayEnd, ECC_Visibility, QueryParams))
		{
			// Place scaffolding at hit location
			FTransform ScaffoldTransform;
			ScaffoldTransform.SetLocation(Hit.ImpactPoint);
			ScaffoldTransform.SetRotation(FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f).Quaternion());
			BlightScaffoldingHISM->AddInstance(ScaffoldTransform, true);
			PlacedCount++;

			// Damage ecosystem — find and destroy nearby flora
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
			if (PlayerPawn)
			{
				UEcosystemArmorComponent* Ecosystem = PlayerPawn->FindComponentByClass<UEcosystemArmorComponent>();
				if (Ecosystem && Ecosystem->FloraHISM && Ecosystem->FloraHISM->GetInstanceCount() > 0)
				{
					TArray<int32> OverlappingInstances = Ecosystem->FloraHISM->GetInstancesOverlappingSphere(
						Hit.ImpactPoint, TreeDestructionRadius);

					// Remove in reverse order to preserve indices
					OverlappingInstances.Sort([](int32 A, int32 B) { return A > B; });
					for (int32 InstanceIdx : OverlappingInstances)
					{
						Ecosystem->FloraHISM->RemoveInstance(InstanceIdx);
						TreesDestroyed++;
					}
				}
			}
		}
	}

	const int32 TotalCorrupted = FCellularBlightGrid::CountCorrupted(BlightGrid.Grid);
	UE_LOG(LogOrogeny, Warning,
		TEXT("=== IRON BLIGHT: Expanded +%d cells (%d total). Placed %d scaffolds, destroyed %d trees ==="),
		NewCells.Num(), TotalCorrupted, PlacedCount, TreesDestroyed);
}
