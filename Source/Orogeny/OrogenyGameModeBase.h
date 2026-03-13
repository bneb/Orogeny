// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OrogenyGameModeBase.generated.h"

/**
 * AOrogenyGameModeBase
 *
 * Foundation Game Mode for Orogeny. Manages:
 * - Default pawn class (ATitanCharacter)
 * - Game state lifecycle (Day 12: Win/Loss conditions)
 * - Supercell encounter timing
 *
 * The game loop is simple:
 *   - Supercell overlaps Player for >10s → Loss
 *   - Player survives 15 minutes → Win
 */
UCLASS()
class OROGENY_API AOrogenyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOrogenyGameModeBase();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// -----------------------------------------------------------------------
	// Game State (Day 12 - Stubbed)
	// -----------------------------------------------------------------------

	/** Total elapsed game time in seconds */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|GameState")
	float ElapsedGameTime = 0.0f;

	/** Duration in seconds the Supercell has been overlapping the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|GameState")
	float SupercellOverlapDuration = 0.0f;

	/** Win condition: survive for this many seconds (default 900 = 15 min) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|GameState")
	float WinSurvivalTime = 900.0f;

	/** Loss condition: Supercell overlaps player for this many seconds (default 10) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Orogeny|GameState")
	float LossOverlapThreshold = 10.0f;
};
