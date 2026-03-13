// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OrographicLiftComponent.generated.h"

class ASupercell_Actor;

/**
 * UOrographicLiftComponent
 *
 * Day 9: The Titan's Offensive Capability — Orographic Lift
 *
 * Executes a massive Area of Effect (AOE) sphere overlap to find
 * ASupercell_Actor instances within range and weakens them.
 *
 * ARCHITECTURE:
 * - ExecuteLift() is called by UAnimNotify_ExecuteLift on the "Thrust" montage.
 * - Uses UWorld::OverlapMultiByObjectType for the AOE sphere query.
 * - Each hit Supercell receives WeakenStorm(SpeedDamage, OpacityDamage).
 *
 * DESIGN:
 * - LiftRadius = 30000 cm (300m) — massive AOE matching Titan scale.
 * - SpeedDamage = 15 cm/s — takes ~3 hits to reduce storm from 50 to floor (10).
 * - OpacityDamage = 0.25 — takes ~3 hits to reduce from 1.0 to floor (0.2).
 */
UCLASS(ClassGroup=(Orogeny), meta=(BlueprintSpawnableComponent))
class OROGENY_API UOrographicLiftComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOrographicLiftComponent();

	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	static constexpr float DEFAULT_LIFT_RADIUS = 30000.0f;
	static constexpr float DEFAULT_SPEED_DAMAGE = 15.0f;
	static constexpr float DEFAULT_OPACITY_DAMAGE = 0.25f;

	// -----------------------------------------------------------------------
	// Configuration Properties
	// -----------------------------------------------------------------------

	/** Radius of the AOE sphere overlap (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Combat",
		meta = (ClampMin = "0.0"))
	float LiftRadius = DEFAULT_LIFT_RADIUS;

	/** Speed reduction applied to each hit Supercell per lift. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Combat",
		meta = (ClampMin = "0.0"))
	float SpeedDamage = DEFAULT_SPEED_DAMAGE;

	/** Opacity reduction applied to each hit Supercell per lift. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Combat",
		meta = (ClampMin = "0.0"))
	float OpacityDamage = DEFAULT_OPACITY_DAMAGE;

	/**
	 * Execute the Orographic Lift AOE attack.
	 * Performs a sphere overlap from the owner's location,
	 * finds all ASupercell_Actor instances within LiftRadius,
	 * and calls WeakenStorm on each.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Combat")
	void ExecuteLift();
};
