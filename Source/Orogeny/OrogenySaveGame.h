// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OrogenySaveGame.generated.h"

/**
 * UOrogenySaveGame
 *
 * Sprint 14: Serialized World State — The Mountain's Memory
 *
 * Contains every piece of game state required to restore a session.
 * All properties are UPROPERTY-tagged for automatic UE serialization.
 *
 * ARCHITECTURE:
 *   - Packed by USaveLoadSubsystem::PackSaveData (pure, TDD-testable)
 *   - Written by UGameplayStatics::AsyncSaveGameToSlot
 *   - Read by UGameplayStatics::AsyncLoadGameFromSlot
 */
UCLASS()
class OROGENY_API UOrogenySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UOrogenySaveGame();

	// -----------------------------------------------------------------------
	// Save Metadata
	// -----------------------------------------------------------------------

	/** Slot name for disk I/O */
	UPROPERTY(VisibleAnywhere, Category = "Orogeny|Save")
	FString SaveSlotName;

	/** User index (local player) */
	UPROPERTY(VisibleAnywhere, Category = "Orogeny|Save")
	uint32 UserIndex;

	// -----------------------------------------------------------------------
	// Deep Time State
	// -----------------------------------------------------------------------

	/** Current day in the Deep Time chronology (double for century-scale precision) */
	UPROPERTY(VisibleAnywhere, Category = "Orogeny|Save")
	double SavedDeepTimeDay;

	// -----------------------------------------------------------------------
	// Ecosystem State
	// -----------------------------------------------------------------------

	/** Ecosystem health [0.0, 1.0] */
	UPROPERTY(VisibleAnywhere, Category = "Orogeny|Save")
	float SavedEcosystemHealth;

	// -----------------------------------------------------------------------
	// Titan State
	// -----------------------------------------------------------------------

	/** Titan world location */
	UPROPERTY(VisibleAnywhere, Category = "Orogeny|Save")
	FVector SavedTitanLocation;

	// -----------------------------------------------------------------------
	// Blight State
	// -----------------------------------------------------------------------

	/** Active corrupted cell coordinates (compressed from TMap<FIntPoint, bool>) */
	UPROPERTY(VisibleAnywhere, Category = "Orogeny|Save")
	TArray<FIntPoint> SavedBlightGrid;
};
