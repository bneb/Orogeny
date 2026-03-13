// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TitanSubductionComponent.generated.h"

/**
 * UTitanSubductionComponent
 *
 * Sprint 4: Subduction Mechanics — The Mountain Sinks and Crushes
 *
 * The Titan plunges its Z-axis into the terrain, grinding downward
 * with deliberate, linear momentum. This is not a float; it is
 * a tectonic event.
 *
 * MECHANICS:
 *   - ToggleSubduction(): flips target state, begins transition.
 *   - Linear interpolation (FInterpConstantTo): heavy, predictable,
 *     no ease-in/ease-out. The Mountain does not float.
 *   - At full depth: BoxOverlap crushes Iron Blight scaffolding.
 *   - During subduction: Ecosystem healing multiplier accelerates regrowth.
 *
 * ARCHITECTURE:
 *   - CalculateSubductionZ is static & pure — TDD without a UWorld.
 *   - BaseZ captured on BeginPlay as the "surface" reference.
 *   - Transition completes when CurrentZ == TargetZ (within tolerance).
 */
UCLASS(ClassGroup = (Orogeny), meta = (BlueprintSpawnableComponent))
class OROGENY_API UTitanSubductionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTitanSubductionComponent();

	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Tolerance for snap-to-target (prevents Z jitter) */
	static constexpr float SNAP_TOLERANCE = 0.1f;

	// -----------------------------------------------------------------------
	// State
	// -----------------------------------------------------------------------

	/** True when the Titan is in the subducted (sunk) state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Subduction")
	bool bIsSubducted = false;

	/** True during the transition animation (sinking or emerging) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Subduction")
	bool bIsTransitioning = false;

	// -----------------------------------------------------------------------
	// Tuning
	// -----------------------------------------------------------------------

	/** How far below surface the Titan sinks (negative = downward) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Subduction")
	float SubductionDepth = -15000.0f;

	/** Speed of Z-axis travel in cm/s (linear, no easing) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Subduction")
	float TransitionSpeed = 2000.0f;

	/** Multiplier applied to Ecosystem growth rate during subduction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Subduction")
	float HealingMultiplier = 500.0f;

	/** Radius for SphereOverlap to crush blight at full depth */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Subduction")
	float CrushRadius = 5000.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static for TDD
	// -----------------------------------------------------------------------

	/**
	 * Calculate the next Z position during subduction transition.
	 * Uses FMath::FInterpConstantTo for linear, heavy movement.
	 *
	 * @param CurrentZ   Current Z position.
	 * @param TargetZ    Target Z position.
	 * @param Speed      Travel speed in cm/s.
	 * @param DeltaTime  Frame delta time.
	 * @return           New Z position (snaps to target if within tolerance).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|Subduction")
	static float CalculateSubductionZ(float CurrentZ, float TargetZ, float Speed, float DeltaTime);

	// -----------------------------------------------------------------------
	// State Control
	// -----------------------------------------------------------------------

	/** Toggle subduction state. Begins transition. */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Subduction")
	void ToggleSubduction();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Surface Z captured at BeginPlay — the "ground level" reference */
	float BaseZ = 0.0f;

	/** Execute blight destruction at full subduction depth */
	void CrushBlightAtDepth();
};
