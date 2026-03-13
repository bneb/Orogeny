// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Supercell_Actor.generated.h"

class USphereComponent;
class UVolumetricCloudComponent;

/**
 * ASupercell_Actor
 *
 * Day 8: The Antagonist — A colossal, slow-moving storm system.
 *
 * The Supercell tracks a target (the player's Titan) using custom
 * pure-math vector interpolation. No NavMesh, no AI Controller,
 * no UPathFollowingComponent — just raw direction + speed.
 *
 * ARCHITECTURE:
 * - CalculateNextPosition is static & pure — TDD without a UWorld.
 * - Anti-jitter: if distance < step size, snap to target exactly.
 * - MovementSpeed = 50 cm/s = 1/3rd of the Titan's 150 MaxWalkSpeed.
 * - StormCore radius = 20000 (200m) — this is a continent-scale threat.
 *
 * TRACKING MATH:
 *   Direction = Normalize(Target - Current)
 *   Step = Direction * Speed * DeltaTime
 *   if (Distance < StepSize) → snap to Target (anti-overshoot)
 *   else → Current + Step
 */
UCLASS()
class OROGENY_API ASupercell_Actor : public AActor
{
	GENERATED_BODY()

public:
	ASupercell_Actor();

	// -----------------------------------------------------------------------
	// Design Constants — exposed for TDD verification
	// -----------------------------------------------------------------------

	static constexpr float DEFAULT_MOVEMENT_SPEED = 50.0f;
	static constexpr float DEFAULT_STORM_CORE_RADIUS = 20000.0f;
	static constexpr float MIN_MOVEMENT_SPEED = 10.0f;
	static constexpr float MIN_CLOUD_OPACITY = 0.2f;

	// -----------------------------------------------------------------------
	// Components
	// -----------------------------------------------------------------------

	/** The root collision sphere — radius 20000 (massive storm core). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Storm")
	TObjectPtr<USphereComponent> StormCore;

	/** Volumetric cloud visuals — assigned/styled by Tech Artist in Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Storm")
	TObjectPtr<UVolumetricCloudComponent> CloudVisuals;

	// -----------------------------------------------------------------------
	// Configuration Properties
	// -----------------------------------------------------------------------

	/** How fast the Supercell moves toward its target (cm/s).
	 *  Default 50 = 1/3rd the Titan's max speed. Slow, creeping dread. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Storm Dynamics",
		meta = (ClampMin = "0.0"))
	float MovementSpeed = DEFAULT_MOVEMENT_SPEED;

	/** The actor this storm is tracking. Set from Level Blueprint or BeginPlay. */
	UPROPERTY(Transient, BlueprintReadWrite, Category = "Orogeny|Storm Dynamics")
	TObjectPtr<AActor> TargetActor;

	/** Cloud visual density [0.2, 1.0]. Reduced by Orographic Lift combat.
	 *  Never drops below 0.2 — the storm is always a lingering threat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Storm Dynamics",
		meta = (ClampMin = "0.2", ClampMax = "1.0"))
	float CloudOpacity = 1.0f;

	// -----------------------------------------------------------------------
	// Combat — Day 9: Orographic Lift Weakening
	// -----------------------------------------------------------------------

	/**
	 * Weaken the storm by reducing its speed and opacity.
	 * Both values are floor-clamped: Speed >= 10, Opacity >= 0.2.
	 * The storm NEVER fully stops or disappears — always a lingering threat.
	 *
	 * @param SpeedReduction     Amount to subtract from MovementSpeed.
	 * @param OpacityReduction   Amount to subtract from CloudOpacity.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Storm Dynamics")
	void WeakenStorm(float SpeedReduction, float OpacityReduction);

	// -----------------------------------------------------------------------
	// Pure Math — Tracking Interpolation
	// -----------------------------------------------------------------------

	/**
	 * Calculate the next position along the direction toward a target.
	 *
	 * FORMULA:
	 *   Direction = Normalize(Target - Current)
	 *   StepSize = Speed * DeltaTime
	 *   if Distance < StepSize → return Target (anti-jitter snap)
	 *   else → return Current + Direction * StepSize
	 *
	 * This is a static pure function so TDD can call it without a UWorld.
	 *
	 * @param CurrentLocation   The actor's current world position.
	 * @param TargetLocation    The target to track toward.
	 * @param Speed             Movement speed in cm/s.
	 * @param DeltaTime         Frame delta time in seconds.
	 * @return                  The next world position.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|Storm Dynamics")
	static FVector CalculateNextPosition(
		const FVector& CurrentLocation, const FVector& TargetLocation,
		float Speed, float DeltaTime);

	virtual void Tick(float DeltaTime) override;
};
