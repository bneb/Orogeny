// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OrogenyPlayerController.generated.h"

class UOrogenyHUDWidget;

/**
 * AOrogenyPlayerController
 *
 * Sprint 12: Player Controller with HUD management.
 *
 * ARCHITECTURE:
 *   - BeginPlay creates and adds UOrogenyHUDWidget to viewport.
 *   - HUDWidgetClass is assigned in the Editor (Widget Blueprint
 *     derived from UOrogenyHUDWidget).
 *   - ActiveHUD is the live widget instance.
 */
UCLASS()
class OROGENY_API AOrogenyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// HUD Configuration
	// -----------------------------------------------------------------------

	/** Widget Blueprint class to spawn as the HUD */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|UI")
	TSubclassOf<UOrogenyHUDWidget> HUDWidgetClass;

	/** Live HUD widget instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|UI")
	TObjectPtr<UOrogenyHUDWidget> ActiveHUD;

protected:
	virtual void BeginPlay() override;
};
