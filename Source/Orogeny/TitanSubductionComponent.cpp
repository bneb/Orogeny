// Copyright Orogeny. All Rights Reserved.

#include "TitanSubductionComponent.h"
#include "Orogeny.h"
#include "IronBlightSeed.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// ============================================================================
// Constructor
// ============================================================================

UTitanSubductionComponent::UTitanSubductionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// ============================================================================
// Pure Math — CalculateSubductionZ
// ============================================================================
// FMath::FInterpConstantTo: linear, constant-speed interpolation.
// The Mountain does not ease. It grinds.
//
// PROOF:
//   Sinking:  Current=0, Target=-15000, Speed=2000, Δt=1 → -2000
//   Emerging: Current=-15000, Target=0, Speed=2000, Δt=1 → -13000
//   Snap:     Current=-14500, Target=-15000, Speed=2000, Δt=1 → -15000
//             (distance 500 < step 2000, snaps to target)
// ============================================================================

float UTitanSubductionComponent::CalculateSubductionZ(
	float CurrentZ, float TargetZ, float Speed, float DeltaTime)
{
	return FMath::FInterpConstantTo(CurrentZ, TargetZ, DeltaTime, Speed);
}

// ============================================================================
// ToggleSubduction — Flip target state
// ============================================================================

void UTitanSubductionComponent::ToggleSubduction()
{
	bIsSubducted = !bIsSubducted;
	bIsTransitioning = true;

	UE_LOG(LogOrogeny, Log, TEXT("Subduction toggled: %s (BaseZ=%.0f, Depth=%.0f)"),
		bIsSubducted ? TEXT("SINKING") : TEXT("EMERGING"),
		BaseZ, SubductionDepth);
}

// ============================================================================
// BeginPlay — Capture surface reference
// ============================================================================

void UTitanSubductionComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		BaseZ = Owner->GetActorLocation().Z;
		UE_LOG(LogOrogeny, Log,
			TEXT("UTitanSubductionComponent: BaseZ captured at %.0f. Depth=%.0f, Speed=%.0f"),
			BaseZ, SubductionDepth, TransitionSpeed);
	}
}

// ============================================================================
// Tick — Drive Z-axis transition
// ============================================================================

void UTitanSubductionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsTransitioning)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Determine target Z
	const float TargetZ = bIsSubducted ? (BaseZ + SubductionDepth) : BaseZ;

	// Current location
	FVector Location = Owner->GetActorLocation();
	const float CurrentZ = Location.Z;

	// Calculate next Z
	const float NewZ = CalculateSubductionZ(CurrentZ, TargetZ, TransitionSpeed, DeltaTime);

	// Apply
	Location.Z = NewZ;
	Owner->SetActorLocation(Location);

	// Check if transition is complete
	if (FMath::IsNearlyEqual(NewZ, TargetZ, SNAP_TOLERANCE))
	{
		bIsTransitioning = false;
		Location.Z = TargetZ; // Final snap
		Owner->SetActorLocation(Location);

		UE_LOG(LogOrogeny, Log, TEXT("Subduction transition complete. Z=%.0f, bIsSubducted=%s"),
			TargetZ, bIsSubducted ? TEXT("true") : TEXT("false"));

		// If we just finished sinking, crush the blight
		if (bIsSubducted)
		{
			CrushBlightAtDepth();
		}
	}
}

// ============================================================================
// CrushBlightAtDepth — Destroy blight within crush radius
// ============================================================================

void UTitanSubductionComponent::CrushBlightAtDepth()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return;
	}

	const FVector CrushCenter = Owner->GetActorLocation();

	// Find all IronBlightSeed actors and destroy overlapping scaffolding
	TArray<AActor*> BlightSeeds;
	UGameplayStatics::GetAllActorsOfClass(World, AIronBlightSeed::StaticClass(), BlightSeeds);

	int32 TotalDestroyed = 0;
	for (AActor* SeedActor : BlightSeeds)
	{
		AIronBlightSeed* Seed = Cast<AIronBlightSeed>(SeedActor);
		if (!Seed || !Seed->BlightScaffoldingHISM)
		{
			continue;
		}

		TArray<int32> OverlappingInstances = Seed->BlightScaffoldingHISM->GetInstancesOverlappingSphere(
			CrushCenter, CrushRadius);

		// Remove in reverse order
		OverlappingInstances.Sort([](int32 A, int32 B) { return A > B; });
		for (int32 Idx : OverlappingInstances)
		{
			Seed->BlightScaffoldingHISM->RemoveInstance(Idx);
			TotalDestroyed++;
		}
	}

	if (TotalDestroyed > 0)
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("=== SUBDUCTION CRUSH: Destroyed %d blight scaffolds within %.0f radius ==="),
			TotalDestroyed, CrushRadius);
	}
}
