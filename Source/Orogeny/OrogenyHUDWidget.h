// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OrogenyHUDWidget.generated.h"

/**
 * UOrogenyHUDWidget
 *
 * Sprint 12: UMG HUD Data Backend — The Player's Window
 *
 * Base C++ class for the in-game HUD. Exposes Blueprint-callable
 * getters that read live game state and return display-ready values.
 *
 * ARCHITECTURE:
 *   - No Tick. Getters are polled by UMG data binding.
 *   - FUIMath handles all formatting (proven via TDD).
 *   - UI Artist creates a Widget Blueprint derived from this class.
 *
 * BINDING PATTERN:
 *   Text Block → Bind → Create Binding → Call GetDisplayCentury()
 *   Progress Bar → Bind → Create Binding → Call GetDisplayBlightThreat()
 */
UCLASS()
class OROGENY_API UOrogenyHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// Blueprint-Callable Getters (Data Binding)
	// -----------------------------------------------------------------------

	/**
	 * Get the current century (1-indexed) from Deep Time.
	 * "Century 1" = Year 0-100.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|UI")
	int32 GetDisplayCentury() const;

	/**
	 * Get ecosystem health as an integer percentage [0, 100].
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|UI")
	int32 GetDisplayHealth() const;

	/**
	 * Get blight corruption as a ratio [0.0, 1.0].
	 * Suitable for progress bar fill.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|UI")
	float GetDisplayBlightThreat() const;
};
