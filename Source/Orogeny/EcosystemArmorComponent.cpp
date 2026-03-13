// Copyright Orogeny. All Rights Reserved.

#include "EcosystemArmorComponent.h"
#include "Orogeny.h"
#include "DeepTimeSubsystem.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// ============================================================================
// Constructor
// ============================================================================

UEcosystemArmorComponent::UEcosystemArmorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // 10Hz check — growth is monthly, no need for 60Hz

	// Create the HISM as a subobject
	FloraHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FloraHISM"));
}

// ============================================================================
// Pure Math — IsValidGrowthLocation
// ============================================================================
// Dot product of SurfaceNormal · UP gives cos(angle).
// acos(dot) gives the slope angle in radians → convert to degrees.
//
// PROOF:
//   Normal (0,0,1) · UP (0,0,1) = 1.0 → acos(1) = 0° → Valid (≤45°)
//   Normal (1,0,0) · UP (0,0,1) = 0.0 → acos(0) = 90° → Invalid (>45°)
//   Normal (0.707,0,0.707) · UP = 0.707 → acos(0.707) = 45° → Valid (≤45°)
// ============================================================================

bool UEcosystemArmorComponent::IsValidGrowthLocation(
	const FVector& SurfaceNormal, float ZHeight,
	float MaxSlope, float MinHeight, float MaxHeight)
{
	// Slope check: dot product with world UP
	const FVector UpVector(0.0f, 0.0f, 1.0f);
	const float DotProduct = FVector::DotProduct(SurfaceNormal.GetSafeNormal(), UpVector);
	const float SlopeAngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f)));

	// Must satisfy ALL conditions
	const bool bSlopeValid = SlopeAngleDegrees <= MaxSlope;
	const bool bAboveMin = ZHeight >= MinHeight;
	const bool bBelowMax = ZHeight <= MaxHeight;

	return bSlopeValid && bAboveMin && bBelowMax;
}

// ============================================================================
// Pure Math — CalculateHealth
// ============================================================================
// Simple normalized ratio, clamped.
// ============================================================================

float UEcosystemArmorComponent::CalculateHealth(int32 CurrentInstances, int32 InMaxInstances)
{
	if (InMaxInstances <= 0)
	{
		return 0.0f;
	}
	return FMath::Clamp(
		static_cast<float>(CurrentInstances) / static_cast<float>(InMaxInstances),
		0.0f, 1.0f);
}

// ============================================================================
// BeginPlay
// ============================================================================

void UEcosystemArmorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (FloraHISM)
	{
		FloraHISM->SetupAttachment(this);
		FloraHISM->RegisterComponent();
		FloraHISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		FloraHISM->SetCastShadow(true);
		FloraHISM->bAffectDistanceFieldLighting = false; // Performance

		UE_LOG(LogOrogeny, Log,
			TEXT("UEcosystemArmorComponent: HISM initialized. MaxInstances=%d, MaxSlope=%.1f°, AltRange=[%.0f, %.0f]"),
			MaxInstances, MaxSlopeAngle, MinGrowthAltitude, MaxGrowthAltitude);
	}

	LastGrowthDay = 0.0;
}

// ============================================================================
// Tick — Check Deep Time for Monthly Growth
// ============================================================================

void UEcosystemArmorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Query the Deep Time Subsystem
	UDeepTimeSubsystem* DeepTime = World->GetSubsystem<UDeepTimeSubsystem>();
	if (!DeepTime)
	{
		return;
	}

	// Has a full in-game month passed?
	const double CurrentDay = DeepTime->CurrentDay;
	if (CurrentDay - LastGrowthDay >= GROWTH_INTERVAL_DAYS)
	{
		LastGrowthDay = CurrentDay;
		ExecuteGrowthTick();

		// Update health
		const int32 CurrentCount = FloraHISM ? FloraHISM->GetInstanceCount() : 0;
		EcosystemHealth = CalculateHealth(CurrentCount, MaxInstances);
	}
}

// ============================================================================
// Growth Tick — Raycast and Place Trees
// ============================================================================

void UEcosystemArmorComponent::ExecuteGrowthTick()
{
	if (!FloraHISM || !FloraHISM->GetStaticMesh())
	{
		return;
	}

	if (FloraHISM->GetInstanceCount() >= MaxInstances)
	{
		return; // Forest is fully grown
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Get the owner's bounding box for raycast origins
	FVector Origin;
	FVector BoxExtent;
	Owner->GetActorBounds(false, Origin, BoxExtent);

	// Attempt multiple placements per growth tick
	const int32 AttemptsPerTick = FMath::Min(100, MaxInstances - FloraHISM->GetInstanceCount());
	int32 PlacedThisTick = 0;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredComponent(FloraHISM.Get());
	QueryParams.bTraceComplex = true;

	for (int32 i = 0; i < AttemptsPerTick; ++i)
	{
		// Random point above the owner's bounding box
		const float RandX = FMath::FRandRange(-BoxExtent.X, BoxExtent.X);
		const float RandY = FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y);
		const FVector RayStart = Origin + FVector(RandX, RandY, BoxExtent.Z + 1000.0f);
		const FVector RayEnd = Origin + FVector(RandX, RandY, -BoxExtent.Z - 1000.0f);

		FHitResult Hit;
		if (World->LineTraceSingleByChannel(Hit, RayStart, RayEnd, ECC_Visibility, QueryParams))
		{
			if (IsValidGrowthLocation(Hit.ImpactNormal, Hit.ImpactPoint.Z,
				MaxSlopeAngle, MinGrowthAltitude, MaxGrowthAltitude))
			{
				// Place tree at hit location, aligned to surface normal
				FRotator TreeRotation = Hit.ImpactNormal.Rotation();
				TreeRotation.Pitch -= 90.0f; // Align trunk to surface normal

				// Random yaw rotation for visual variety
				TreeRotation.Yaw = FMath::FRandRange(0.0f, 360.0f);

				// Random scale for visual variety
				const float Scale = FMath::FRandRange(0.8f, 1.2f);

				FTransform TreeTransform;
				TreeTransform.SetLocation(Hit.ImpactPoint);
				TreeTransform.SetRotation(TreeRotation.Quaternion());
				TreeTransform.SetScale3D(FVector(Scale));

				FloraHISM->AddInstance(TreeTransform, /*bWorldSpace=*/true);
				PlacedThisTick++;
			}
		}
	}

	if (PlacedThisTick > 0)
	{
		UE_LOG(LogOrogeny, Log,
			TEXT("EcosystemArmor: Grew %d trees. Total: %d / %d (Health: %.2f)"),
			PlacedThisTick, FloraHISM->GetInstanceCount(), MaxInstances, EcosystemHealth);
	}
}
