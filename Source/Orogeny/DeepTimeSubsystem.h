// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DeepTimeSubsystem.generated.h"

class UMaterialParameterCollection;

/**
 * Delegate fired when a new in-game year begins.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewYear, int32, YearNumber);

/**
 * Delegate fired when a new in-game century begins.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewCentury, int32, CenturyNumber);

/**
 * UDeepTimeSubsystem
 *
 * Sprint 1: The "Deep Time" Chrono-Tick Architecture
 *
 * A decoupled time system that allows centuries to pass in seconds
 * visually, without breaking the 60hz physics tick or the 16.6ms
 * frame budget. Time acceleration is dynamically linked to the
 * Titan's movement velocity.
 *
 * ARCHITECTURE:
 *   - UWorldSubsystem: auto-singleton per UWorld, clean lifecycle.
 *   - All math is static & pure — TDD without a UWorld.
 *   - double precision for CurrentDay — prevents float precision loss
 *     over centuries of accumulated time.
 *   - MPC bridge pushes SunAngle, SeasonAlpha, TimeScale to materials
 *     every frame without Blueprint overhead.
 *
 * TIME SCALE MAPPING (velocity → deep time):
 *   Velocity 0%   → TimeScale 1.0     → 1 day per minute
 *   Velocity 25%  → TimeScale ~9,132  → ~25 years per minute
 *   Velocity 50%  → TimeScale ~18,263 → ~50 years per minute
 *   Velocity 100% → TimeScale 36,525  → 1 century per minute
 *
 * SUN CYCLE:
 *   Fractional day → angle [0, 360)
 *   0.0 = midnight (0°), 0.25 = sunrise (90°),
 *   0.5 = noon (180°), 0.75 = sunset (270°)
 *
 * SEASONS:
 *   Period = 365.25 days
 *   0.0 = winter solstice, 0.5 = summer solstice
 */
UCLASS()
class OROGENY_API UDeepTimeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// Deep Time Constants
	// -----------------------------------------------------------------------

	/** Real seconds per in-game day when the Titan is stationary */
	static constexpr float BASE_DAY_DURATION = 60.0f;

	/** Real seconds per in-game century at maximum velocity */
	static constexpr float MAX_CENTURY_DURATION = 60.0f;

	/** Days per century (365.25 × 100) */
	static constexpr float DAYS_PER_CENTURY = 36525.0f;

	/**
	 * Maximum time scale multiplier.
	 * At rest:  1 day = 60s  → DaysPerSecond = 1/60
	 * At max:   36525 days = 60s → DaysPerSecond = 608.75
	 * MaxTimeScale = 608.75 / (1/60) = 36525.0
	 */
	static constexpr float MAX_TIME_SCALE = DAYS_PER_CENTURY;

	/** Days per year for season calculation */
	static constexpr double DAYS_PER_YEAR = 365.25;

	// -----------------------------------------------------------------------
	// Deep Time State
	// -----------------------------------------------------------------------

	/** Current in-game day count (fractional — 1.5 = noon of day 2) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|DeepTime")
	double CurrentDay = 0.0;

	/** Current time scale multiplier (1.0 = base, MAX_TIME_SCALE = max) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|DeepTime")
	float CurrentTimeScale = 1.0f;

	/** Current velocity alpha driving time scale (0.0-1.0) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|DeepTime")
	float CurrentVelocityAlpha = 0.0f;

	/** Current sun angle in degrees [0, 360), derived from fractional day */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|DeepTime")
	float CurrentSunAngle = 0.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Static Functions for TDD
	// -----------------------------------------------------------------------

	/**
	 * Map velocity alpha [0, 1] to time scale [1, MAX_TIME_SCALE].
	 * Linear interpolation. Clamped input.
	 *
	 * @param VelocityAlpha  Normalized speed (0-1).
	 * @return               Time scale multiplier.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|DeepTime")
	static float CalculateTimeScale(float VelocityAlpha);

	/**
	 * Advance the current day by a frame's worth of deep time.
	 *
	 * @param CurrentDayValue   Current fractional day count.
	 * @param TimeScale         Current time scale multiplier.
	 * @param DeltaTime         Real frame delta time in seconds.
	 * @param BaseDayDuration   Real seconds per in-game day at base rate.
	 * @return                  New fractional day count.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|DeepTime")
	static double AdvanceDay(double CurrentDayValue, float TimeScale, float DeltaTime, float BaseDayDuration);

	/**
	 * Convert fractional day to sun angle (0-360 degrees).
	 *
	 * @param FractionalDay  Current day value (fractional part used).
	 * @return               Sun angle in degrees [0, 360).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|DeepTime")
	static float CalculateSunAngle(double FractionalDay);

	/**
	 * Convert fractional day to seasonal position [0, 1).
	 * Period = 365.25 days.
	 *
	 * @param FractionalDay  Current total day count.
	 * @return               Season alpha [0, 1).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|DeepTime")
	static float CalculateSeasonAlpha(double FractionalDay);

	// -----------------------------------------------------------------------
	// Subsystem Lifecycle (UTickableWorldSubsystem)
	// -----------------------------------------------------------------------

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	// -----------------------------------------------------------------------
	// Material Parameter Collection Bridge
	// -----------------------------------------------------------------------

	/** MPC updated every tick for material reactions to Deep Time */
	UPROPERTY(EditAnywhere, Category = "Orogeny|DeepTime")
	TObjectPtr<UMaterialParameterCollection> DeepTimeMPC;

	UPROPERTY(EditAnywhere, Category = "Orogeny|DeepTime")
	FName MPCSunAngleParam = "SunAngle";

	UPROPERTY(EditAnywhere, Category = "Orogeny|DeepTime")
	FName MPCSeasonAlphaParam = "SeasonAlpha";

	UPROPERTY(EditAnywhere, Category = "Orogeny|DeepTime")
	FName MPCTimeScaleParam = "TimeScale";

	// -----------------------------------------------------------------------
	// Sun Light Control
	// -----------------------------------------------------------------------

	/** Actor tag for the directional light to drive */
	UPROPERTY(EditAnywhere, Category = "Orogeny|DeepTime")
	FName SunLightTag = "DeepTimeSun";

	// -----------------------------------------------------------------------
	// Milestone Delegates
	// -----------------------------------------------------------------------

	/** Fires when a new in-game year begins (every 365.25 days) */
	UPROPERTY(BlueprintAssignable, Category = "Orogeny|DeepTime")
	FOnNewYear OnNewYear;

	/** Fires when a new in-game century begins (every 36,525 days) */
	UPROPERTY(BlueprintAssignable, Category = "Orogeny|DeepTime")
	FOnNewCentury OnNewCentury;

private:
	/** Cached reference to the tagged sun light actor */
	UPROPERTY(Transient)
	TObjectPtr<AActor> SunLightActor;

	/** Last broadcast year number (prevents duplicate broadcasts) */
	int32 LastBroadcastYear = -1;

	/** Last broadcast century number */
	int32 LastBroadcastCentury = -1;

	/** Update the tagged directional light rotation from CurrentSunAngle */
	void UpdateSunLight();

	/** Check and broadcast year/century milestone delegates */
	void CheckMilestones();
};
