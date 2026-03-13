// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OrogenyGameModeBase.generated.h"

class UOrogenyDifficultyPreset;

/**
 * EOrogenyGameState
 *
 * The three possible states of the Orogeny game loop.
 * Defeat ALWAYS takes priority over Victory if both conditions
 * are met simultaneously (the storm claims all).
 */
UENUM(BlueprintType)
enum class EOrogenyGameState : uint8
{
	Playing  UMETA(DisplayName = "Playing"),
	Victory  UMETA(DisplayName = "Victory"),
	Defeat   UMETA(DisplayName = "Defeat")
};

/**
 * AOrogenyGameModeBase
 *
 * Day 12: The Core Game Loop — Survival vs. The Storm
 *
 * WIN CONDITION:  Survive RequiredSurvivalTime seconds (default 900 = 15 min)
 * LOSS CONDITION: Spend MaxStormExposure seconds inside the Supercell (default 10s)
 *
 * RECOVERY MECHANIC:
 *   If the Titan escapes the storm, exposure decays at 1:1 rate.
 *   This creates a risk/reward dynamic: the player can flirt with
 *   the storm's edge, dipping in and out while managing exposure.
 *
 * ARCHITECTURE:
 *   - CalculateExposureDelta is static & pure — TDD without a UWorld.
 *   - EvaluateGameState is static & pure — TDD without a UWorld.
 *   - Tick drives the loop: increment survival, check storm proximity,
 *     update exposure, evaluate state, trigger win/loss.
 *   - No physics overlaps — pure distance math vs. StormCore radius.
 */
UCLASS()
class OROGENY_API AOrogenyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOrogenyGameModeBase();

	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	static constexpr float DEFAULT_SURVIVAL_TIME = 900.0f;
	static constexpr float DEFAULT_MAX_EXPOSURE = 10.0f;
	static constexpr float DEFAULT_TARGET_CENTURIES = 10.0f;

	// -----------------------------------------------------------------------
	// Difficulty Preset (Sprint 15)
	// -----------------------------------------------------------------------

	/** Active difficulty preset — assign a Data Asset in the Editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Difficulty")
	TObjectPtr<UOrogenyDifficultyPreset> ActiveDifficulty;

	// -----------------------------------------------------------------------
	// Game State Properties
	// -----------------------------------------------------------------------

	/** Current state of the game loop */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|GameState")
	EOrogenyGameState CurrentGameState = EOrogenyGameState::Playing;

	/** How long the player must survive to win (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|GameState")
	float RequiredSurvivalTime = DEFAULT_SURVIVAL_TIME;

	/** Current elapsed survival time (seconds) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|GameState")
	float CurrentSurvivalTime = 0.0f;

	/** Maximum storm exposure before defeat (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|GameState")
	float MaxStormExposure = DEFAULT_MAX_EXPOSURE;

	/** Current accumulated storm exposure (seconds) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|GameState")
	float CurrentStormExposure = 0.0f;

	// -----------------------------------------------------------------------
	// Sprint 5: Deep Time Game State — Centuries + Ecosystem Health
	// -----------------------------------------------------------------------

	/** Centuries the player must endure to win (default 10 = 1,000 years) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|GameState")
	float TargetSurvivalCenturies = DEFAULT_TARGET_CENTURIES;

	/** Ecosystem health at or below which the Mountain loses */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|GameState")
	float CriticalHealthThreshold = 0.0f;

	/** Current century count (read from DeepTimeSubsystem) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|GameState")
	float CurrentCenturies = 0.0f;

	/** Current ecosystem health (read from EcosystemArmorComponent) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|GameState")
	float CurrentEcosystemHealth = 0.0f;

	// -----------------------------------------------------------------------
	// Pure Math — State Logic (TDD-friendly)
	// -----------------------------------------------------------------------

	/**
	 * Calculate the next exposure value.
	 * Inside storm: exposure increases by DeltaTime.
	 * Outside storm: exposure RECOVERS (decreases) by DeltaTime.
	 * Clamped to [0, MaxExposure].
	 *
	 * @param CurrentExposure   Current accumulated exposure.
	 * @param bIsInsideStorm    Whether the player is inside the storm radius.
	 * @param DeltaTime         Frame delta time.
	 * @param MaxExposure       Maximum exposure before defeat.
	 * @return                  Updated exposure value.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|GameState")
	static float CalculateExposureDelta(
		float CurrentExposure, bool bIsInsideStorm,
		float DeltaTime, float MaxExposure);

	/**
	 * Evaluate the game state based on survival time and exposure.
	 * PRIORITY: Defeat ALWAYS overrides Victory.
	 *
	 * @param SurvivalTime   Total time survived.
	 * @param RequiredTime   Time required for victory.
	 * @param Exposure       Current storm exposure.
	 * @param MaxExposure    Maximum exposure before defeat.
	 * @return               The resulting game state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|GameState")
	static EOrogenyGameState EvaluateGameState(
		float SurvivalTime, float RequiredTime,
		float Exposure, float MaxExposure);

	/**
	 * Evaluate game state using Deep Time centuries and Ecosystem health.
	 * Sprint 5: Replaces the Day 12 timer for the vertical slice.
	 * PRIORITY: Defeat (health <= critical) ALWAYS overrides Victory.
	 *
	 * @param CurrentCenturies    Centuries survived (CurrentDay / 36525).
	 * @param TargetCenturies     Centuries required for victory.
	 * @param CurrentHealth       Current ecosystem health [0, 1].
	 * @param CriticalHealth      Health threshold for defeat.
	 * @return                    The resulting game state.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|GameState")
	static EOrogenyGameState EvaluateDeepTimeGameState(
		float InCenturies, float InTargetCenturies,
		float InHealth, float InCriticalHealth);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	// -----------------------------------------------------------------------
	// Win/Loss Execution — BlueprintImplementableEvent for UI
	// -----------------------------------------------------------------------

	/** Called once when the player survives long enough. Wire UI in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Orogeny|GameState")
	void OnVictory();

	/** Called once when the storm claims the player. Wire UI in Blueprint. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Orogeny|GameState")
	void OnDefeat();

	/** C++ handler for victory — disables input, then fires Blueprint event. */
	virtual void HandleVictory();

	/** C++ handler for defeat — disables input, then fires Blueprint event. */
	virtual void HandleDefeat();
};
