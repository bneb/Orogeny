// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "SaveLoadSubsystem.generated.h"

class UOrogenySaveGame;

/**
 * USaveLoadSubsystem
 *
 * Sprint 14: Serialization & Persistence — The Mountain's Memory
 *
 * UGameInstanceSubsystem — survives level transitions.
 * Decouples data transfer (PackSaveData, ExtractBlightGrid) from
 * disk I/O (AsyncSave/Load) so the math is TDD-testable.
 *
 * ARCHITECTURE:
 *   - PackSaveData: Pure data copy (game state → save object)
 *   - ExtractBlightGrid: TMap compression (only corrupted cells)
 *   - SaveGame/LoadGame: Async disk I/O via UGameplayStatics
 */
UCLASS()
class OROGENY_API USaveLoadSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// Pure Data Functions (Static, for TDD)
	// -----------------------------------------------------------------------

	/**
	 * Pack live game state into the save object.
	 *
	 * Pure data transfer — no disk I/O, no world access.
	 * Fully testable in headless automation.
	 *
	 * @param SaveObject   Target save game object.
	 * @param CurrentDay   Deep Time elapsed days.
	 * @param EcoHealth    Ecosystem health [0, 1].
	 * @param TitanLoc     Titan world location.
	 * @param BlightCells  Compressed corrupted cell coordinates.
	 */
	static void PackSaveData(UOrogenySaveGame* SaveObject,
		double CurrentDay, float EcoHealth,
		const FVector& TitanLoc,
		const TArray<FIntPoint>& BlightCells);

	/**
	 * Extract corrupted cells from the live cellular automata grid.
	 *
	 * Compresses TMap<FIntPoint, bool> to TArray<FIntPoint> by
	 * only including cells where value == true (corrupted).
	 *
	 * @param LiveGrid  The cellular automata state map.
	 * @return          Array of corrupted cell coordinates.
	 */
	static TArray<FIntPoint> ExtractBlightGrid(
		const TMap<FIntPoint, bool>& LiveGrid);

	// -----------------------------------------------------------------------
	// Disk I/O (Blueprint Callable)
	// -----------------------------------------------------------------------

	/**
	 * Save the current game state to a named slot.
	 * Gathers data from all subsystems and writes async.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|SaveLoad")
	void SaveGame(FString SlotName);

	/**
	 * Load game state from a named slot.
	 * Restores all subsystems and Titan position.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|SaveLoad")
	void LoadGame(FString SlotName);

private:
	/** Callback for async load completion */
	void OnLoadComplete(const FString& SlotName, int32 UserIdx,
		USaveGame* LoadedSaveGame);
};
