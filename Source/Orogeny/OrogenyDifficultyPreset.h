// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OrogenyDifficultyPreset.generated.h"

/**
 * UOrogenyDifficultyPreset
 *
 * Sprint 15: Difficulty Data Asset — The Mountain's Challenge
 *
 * UPrimaryDataAsset — Designer-editable, swappable at runtime.
 * Create instances in the Content Browser for each difficulty tier.
 *
 * ARCHITECTURE:
 *   - Game Designer creates Data Assets: DA_Diff_Story, DA_Diff_Survival, DA_Diff_Endless
 *   - Assigned to AOrogenyGameModeBase::ActiveDifficulty in the Editor
 *   - FTuningMath applies multipliers to base game parameters
 */
UCLASS(BlueprintType)
class OROGENY_API UOrogenyDifficultyPreset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// Identity
	// -----------------------------------------------------------------------

	/** Human-readable preset name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|Difficulty")
	FName PresetName;

	// -----------------------------------------------------------------------
	// Blight Tuning
	// -----------------------------------------------------------------------

	/**
	 * Multiplier applied to the base Blight spread probability.
	 * 1.0 = default. 2.0 = twice as aggressive. 0.5 = half speed.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|Difficulty",
		meta = (ClampMin = "0.01", ClampMax = "10.0"))
	float BlightSpreadMultiplier = 1.0f;

	// -----------------------------------------------------------------------
	// Healing Tuning
	// -----------------------------------------------------------------------

	/**
	 * Multiplier applied to the base Ecosystem healing rate.
	 * 1.0 = default. 1.5 = 50% faster healing. 0.5 = 50% slower.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|Difficulty",
		meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float EcosystemHealMultiplier = 1.0f;

	// -----------------------------------------------------------------------
	// Win Condition Tuning
	// -----------------------------------------------------------------------

	/**
	 * Centuries the mountain must survive to win.
	 * 0.0 = Endless Mode (no victory condition).
	 * 10.0 = default (1,000 years).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|Difficulty",
		meta = (ClampMin = "0.0"))
	float TargetSurvivalCenturies = 10.0f;
};
