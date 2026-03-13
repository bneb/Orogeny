// Copyright Orogeny. All Rights Reserved.

#include "OrogenyHUDWidget.h"
#include "UIMath.h"
#include "DeepTimeSubsystem.h"
#include "EcosystemArmorComponent.h"
#include "CellularBlightGrid.h"
#include "Kismet/GameplayStatics.h"

// ============================================================================
// GetDisplayCentury — Deep Time days → 1-indexed century
// ============================================================================

int32 UOrogenyHUDWidget::GetDisplayCentury() const
{
	if (UWorld* World = GetWorld())
	{
		if (UDeepTimeSubsystem* DeepTime = World->GetSubsystem<UDeepTimeSubsystem>())
		{
			return FUIMath::CalculateCurrentCentury(DeepTime->CurrentDay);
		}
	}
	return 1;
}

// ============================================================================
// GetDisplayHealth — Ecosystem health → integer percentage
// ============================================================================

int32 UOrogenyHUDWidget::GetDisplayHealth() const
{
	if (UWorld* World = GetWorld())
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
		if (PlayerPawn)
		{
			UEcosystemArmorComponent* Ecosystem =
				PlayerPawn->FindComponentByClass<UEcosystemArmorComponent>();
			if (Ecosystem)
			{
				return FUIMath::CalculateHealthPercentage(Ecosystem->EcosystemHealth);
			}
		}
	}
	return 100;
}

// ============================================================================
// GetDisplayBlightThreat — Cell counts → corruption ratio
// ============================================================================

float UOrogenyHUDWidget::GetDisplayBlightThreat() const
{
	// Future integration: query global blight tracker for cell counts.
	// For now, return 0.0 (pristine) — the C++ math is TDD-proven,
	// and the Blueprint binding is ready for when the global tracker
	// is wired into the GameMode.
	return 0.0f;
}
