// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TectonicAtmosphereComponent.generated.h"

class UMaterialParameterCollection;
class UNiagaraComponent;
class UNiagaraSystem;

/**
 * UTectonicAtmosphereComponent
 *
 * Day 7: Atmospheric Wake & VFX Data Binding
 *
 * Calculates a velocity-scaled displacement radius and feeds it to:
 * 1. A Material Parameter Collection (MPC) — for fog/atmosphere displacement
 * 2. A Niagara System — for dust/wake particle intensity
 *
 * ARCHITECTURE:
 * - CalculateDisplacementRadius is static & pure — TDD without a UWorld.
 * - TickComponent reads velocity, computes radius, pushes to MPC + Niagara.
 * - MPC updates are null-safe (tech artist may not have assigned it yet).
 *
 * MATH:
 * Radius = BaseRadius * (1.0 + SpeedAlpha * (Multiplier - 1.0))
 * where SpeedAlpha = Clamp(CurrentSpeed / MaxSpeed, 0, 1)
 *
 * At Speed=0:       Radius = BaseRadius * 1.0 = BaseRadius
 * At Speed=Max:     Radius = BaseRadius * Multiplier
 * At Speed=Max/2:   Radius = BaseRadius * (1.0 + 0.5 * (Mult-1))
 */
UCLASS(ClassGroup=(Orogeny), meta=(BlueprintSpawnableComponent))
class OROGENY_API UTectonicAtmosphereComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UTectonicAtmosphereComponent();

	// -----------------------------------------------------------------------
	// Design Constants — exposed for TDD verification
	// -----------------------------------------------------------------------

	static constexpr float DEFAULT_BASE_RADIUS = 5000.0f;
	static constexpr float DEFAULT_MAX_VELOCITY_MULTIPLIER = 2.0f;

	// -----------------------------------------------------------------------
	// Configuration Properties
	// -----------------------------------------------------------------------

	/** Material Parameter Collection for fog displacement.
	 *  Assign in Blueprint/Editor. Null-safe at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere")
	TObjectPtr<UMaterialParameterCollection> AtmosphereMPC;

	/** MPC vector parameter name for the Titan's world position. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere")
	FName MPCPositionParameterName = "TitanLocation";

	/** MPC scalar parameter name for the displacement radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere")
	FName MPCRadiusParameterName = "DisplacementRadius";

	/** Base displacement radius when stationary (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere",
		meta = (ClampMin = "0.0"))
	float BaseDisplacementRadius = DEFAULT_BASE_RADIUS;

	/** Multiplier applied to BaseRadius at max velocity. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere",
		meta = (ClampMin = "1.0"))
	float MaxVelocityMultiplier = DEFAULT_MAX_VELOCITY_MULTIPLIER;

	/** Niagara asset to spawn for the dust wake. Assign in Editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere")
	TObjectPtr<UNiagaraSystem> DustWakeAsset;

	// -----------------------------------------------------------------------
	// Runtime State
	// -----------------------------------------------------------------------

	/** The spawned Niagara dust wake system. Created in BeginPlay. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Atmosphere")
	TObjectPtr<UNiagaraComponent> DustWakeSystem;

	/** Current calculated displacement radius. Updated each tick. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Orogeny|Atmosphere")
	float CurrentDisplacementRadius = DEFAULT_BASE_RADIUS;

	/** Current wake intensity [0, 1]. Updated each tick. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Orogeny|Atmosphere")
	float CurrentWakeIntensity = 0.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Velocity to Radius
	// -----------------------------------------------------------------------

	/**
	 * Calculate the atmospheric displacement radius from velocity.
	 *
	 * FORMULA:
	 *   SpeedAlpha = Clamp(CurrentSpeed / MaxSpeed, 0, 1)
	 *   Radius = BaseRadius * (1.0 + SpeedAlpha * (Multiplier - 1.0))
	 *
	 * PROOF:
	 *   Speed=0:   Alpha=0 → Radius = Base * 1.0 = Base
	 *   Speed=Max: Alpha=1 → Radius = Base * Mult
	 *   Speed=2x:  Alpha=1 (clamped) → Radius = Base * Mult (no overshoot)
	 *
	 * @param CurrentSpeed   Current character speed.
	 * @param MaxSpeed       The character's max walk speed.
	 * @param BaseRadius     Base displacement radius when stationary.
	 * @param Multiplier     Max multiplier at full speed.
	 * @return               Calculated displacement radius.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|Atmosphere")
	static float CalculateDisplacementRadius(
		float CurrentSpeed, float MaxSpeed,
		float BaseRadius, float Multiplier);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
};
