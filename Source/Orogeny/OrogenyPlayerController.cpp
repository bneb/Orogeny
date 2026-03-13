// Copyright Orogeny. All Rights Reserved.

#include "OrogenyPlayerController.h"
#include "Orogeny.h"
#include "OrogenyHUDWidget.h"
#include "Blueprint/UserWidget.h"

// ============================================================================
// BeginPlay — Create and display HUD
// ============================================================================

void AOrogenyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		ActiveHUD = CreateWidget<UOrogenyHUDWidget>(this, HUDWidgetClass);
		if (ActiveHUD)
		{
			ActiveHUD->AddToViewport();
			UE_LOG(LogOrogeny, Log,
				TEXT("OrogenyPlayerController: HUD widget added to viewport"));
		}
		else
		{
			UE_LOG(LogOrogeny, Error,
				TEXT("OrogenyPlayerController: Failed to create HUD widget!"));
		}
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("OrogenyPlayerController: No HUDWidgetClass assigned!"));
	}
}
