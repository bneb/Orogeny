// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EcosystemArmorComponent.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

/**
 * UEcosystemArmorComponent
 *
 * Sprint 2: Procedural Ecosystem Armor — The Mountain's Living Health
 *
 * Manages the Titan's ecosystem as a HISM-driven forest that grows
 * based on slope angle and altitude constraints. Flora density IS
 * the Titan's health — no abstract HP bars.
 *
 * ARCHITECTURE:
 *   - Pure math validation: IsValidGrowthLocation() is static & TDD-friendly.
 *   - Health normalization: CalculateHealth() returns [0,1] for materials/UI.
 *   - Growth executes on Deep Time month boundaries (every 30 in-game days).
 *   - Raycasts downward from random bounding box positions onto the Titan mesh.
 *   - Only places trees where slope <= MaxSlopeAngle AND altitude in range.
 *
 * GROWTH RULES:
 *   - Flat/gentle slopes (≤45°) between 1000-25000 units: trees grow
 *   - Sheer cliffs (>45°): barren rock, no growth
 *   - Below MinGrowthAltitude: submerged/too low
 *   - Above MaxGrowthAltitude: snow line, too cold
 *
 * PERFORMANCE:
 *   - HISM batches all 50,000 trees into a single draw call via Nanite.
 *   - Growth tick is monthly (in Deep Time), not per-frame.
 *   - No physics on tree instances — visual only.
 */
UCLASS(ClassGroup = (Orogeny), meta = (BlueprintSpawnableComponent))
class OROGENY_API UEcosystemArmorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UEcosystemArmorComponent();

	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** In-game days between growth ticks */
	static constexpr float GROWTH_INTERVAL_DAYS = 30.0f;

	// -----------------------------------------------------------------------
	// Properties
	// -----------------------------------------------------------------------

	/** HISM for all flora instances (single draw call via Nanite) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Ecosystem")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FloraHISM;

	/** Maximum slope angle for tree growth (degrees from vertical) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Ecosystem")
	float MaxSlopeAngle = 45.0f;

	/** Minimum Z-height for growth (below = too low) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Ecosystem")
	float MinGrowthAltitude = 1000.0f;

	/** Maximum Z-height for growth (above = snow line) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Ecosystem")
	float MaxGrowthAltitude = 25000.0f;

	/** Maximum number of flora instances */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Ecosystem")
	int32 MaxInstances = 50000;

	/** Current ecosystem health — normalized [0, 1] */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Ecosystem")
	float EcosystemHealth = 0.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Validate whether a surface location can support tree growth.
	 *
	 * Uses dot product of SurfaceNormal against UP vector to derive
	 * slope angle. Growth is allowed only if:
	 *   1. Slope angle <= MaxSlope (not too steep)
	 *   2. ZHeight >= MinHeight (not too low)
	 *   3. ZHeight <= MaxHeight (not above snow line)
	 *
	 * @param SurfaceNormal  World-space normal of the surface hit.
	 * @param ZHeight        World Z-coordinate of the hit point.
	 * @param MaxSlope       Maximum allowed slope in degrees.
	 * @param MinHeight      Minimum altitude for growth.
	 * @param MaxHeight      Maximum altitude for growth.
	 * @return               True if this location can grow a tree.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|Ecosystem")
	static bool IsValidGrowthLocation(
		const FVector& SurfaceNormal, float ZHeight,
		float MaxSlope, float MinHeight, float MaxHeight);

	/**
	 * Calculate normalized ecosystem health [0, 1].
	 *
	 * @param CurrentInstances  Number of living flora instances.
	 * @param InMaxInstances    Maximum possible instances.
	 * @return                  Health ratio, clamped to [0, 1].
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|Ecosystem")
	static float CalculateHealth(int32 CurrentInstances, int32 InMaxInstances);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Last in-game day a growth tick was executed */
	double LastGrowthDay = 0.0;

	/** Execute a single growth tick — raycast and place trees */
	void ExecuteGrowthTick();
};
