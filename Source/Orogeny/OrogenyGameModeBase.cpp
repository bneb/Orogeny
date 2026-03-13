// Copyright Orogeny. All Rights Reserved.

#include "OrogenyGameModeBase.h"
#include "Orogeny.h"
#include "OrogenyDifficultyPreset.h"
#include "TuningMath.h"
#include "TitanCharacter.h"
#include "Supercell_Actor.h"
#include "DeepTimeSubsystem.h"
#include "EcosystemArmorComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

// ============================================================================
// Constructor
// ============================================================================

AOrogenyGameModeBase::AOrogenyGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = ATitanCharacter::StaticClass();

	UE_LOG(LogOrogeny, Log,
		TEXT("AOrogenyGameModeBase initialized. Win: survive %.0fs. Loss: storm exposure > %.0fs."),
		RequiredSurvivalTime, MaxStormExposure);
}

// ============================================================================
// BeginPlay
// ============================================================================

void AOrogenyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentGameState = EOrogenyGameState::Playing;
	CurrentSurvivalTime = 0.0f;
	CurrentStormExposure = 0.0f;

	// -----------------------------------------------------------------------
	// Sprint 15: Apply Difficulty Preset
	// -----------------------------------------------------------------------
	if (ActiveDifficulty)
	{
		UE_LOG(LogOrogeny, Log,
			TEXT("Difficulty Preset: %s (Blight x%.2f, Heal x%.2f, Centuries=%.1f)"),
			*ActiveDifficulty->PresetName.ToString(),
			ActiveDifficulty->BlightSpreadMultiplier,
			ActiveDifficulty->EcosystemHealMultiplier,
			ActiveDifficulty->TargetSurvivalCenturies);

		// Apply win condition
		if (FTuningMath::IsEndlessMode(ActiveDifficulty->TargetSurvivalCenturies))
		{
			// Endless: set target impossibly high so victory never triggers
			TargetSurvivalCenturies = TNumericLimits<float>::Max();
			UE_LOG(LogOrogeny, Log, TEXT("  Mode: ENDLESS (no victory condition)"));
		}
		else
		{
			TargetSurvivalCenturies = ActiveDifficulty->TargetSurvivalCenturies;
			UE_LOG(LogOrogeny, Log,
				TEXT("  Target: Survive %.1f centuries"),
				TargetSurvivalCenturies);
		}
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("No Difficulty Preset assigned — using defaults"));
	}

	UE_LOG(LogOrogeny, Log,
		TEXT("Game Loop started. RequiredSurvival=%.0fs, MaxExposure=%.0fs"),
		RequiredSurvivalTime, MaxStormExposure);
}

// ============================================================================
// Exposure Delta — The Recovery Mechanic
// ============================================================================
// Inside storm:  exposure += DeltaTime (danger builds)
// Outside storm: exposure -= DeltaTime (recovery!)
// Clamped to [0, MaxExposure]
//
// PROOF (Max=10):
//   (5.0, inside,  1.0) → 6.0  ✓
//   (5.0, outside, 1.0) → 4.0  ✓
//   (9.5, inside,  1.0) → 10.0 ✓ clamped
//   (0.5, outside, 1.0) → 0.0  ✓ clamped
// ============================================================================

float AOrogenyGameModeBase::CalculateExposureDelta(
	float CurrentExposure, bool bIsInsideStorm,
	float DeltaTime, float MaxExposure)
{
	const float Delta = bIsInsideStorm ? DeltaTime : -DeltaTime;
	return FMath::Clamp(CurrentExposure + Delta, 0.0f, MaxExposure);
}

// ============================================================================
// State Evaluation — Defeat ALWAYS overrides Victory
// ============================================================================
// Priority order:
//   1. Defeat (exposure >= max) — the storm claims all
//   2. Victory (survival >= required)
//   3. Playing (neither condition met)
// ============================================================================

EOrogenyGameState AOrogenyGameModeBase::EvaluateGameState(
	float SurvivalTime, float RequiredTime,
	float Exposure, float MaxExposure)
{
	// Defeat ALWAYS takes priority
	if (Exposure >= MaxExposure)
	{
		return EOrogenyGameState::Defeat;
	}

	if (SurvivalTime >= RequiredTime)
	{
		return EOrogenyGameState::Victory;
	}

	return EOrogenyGameState::Playing;
}

// ============================================================================
// State Evaluation — Deep Time (Sprint 5)
// ============================================================================
// Priority order:
//   1. Defeat (health <= critical) — the Blight strips the Mountain bare
//   2. Victory (centuries >= target) — the Mountain has endured
//   3. Playing (neither condition met)
// ============================================================================

EOrogenyGameState AOrogenyGameModeBase::EvaluateDeepTimeGameState(
	float InCenturies, float InTargetCenturies,
	float InHealth, float InCriticalHealth)
{
	// Defeat ALWAYS takes priority — if the Mountain is bare, nature loses
	if (InHealth <= InCriticalHealth)
	{
		return EOrogenyGameState::Defeat;
	}

	if (InCenturies >= InTargetCenturies)
	{
		return EOrogenyGameState::Victory;
	}

	return EOrogenyGameState::Playing;
}

// ============================================================================
// Tick — The Core Game Loop
// ============================================================================

void AOrogenyGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -----------------------------------------------------------------------
	// Early out: game is already over
	// -----------------------------------------------------------------------
	if (CurrentGameState != EOrogenyGameState::Playing)
	{
		return;
	}

	// -----------------------------------------------------------------------
	// Increment survival time
	// -----------------------------------------------------------------------
	CurrentSurvivalTime += DeltaTime;

	// -----------------------------------------------------------------------
	// Find storm proximity (no physics overlap — pure distance math)
	// -----------------------------------------------------------------------
	bool bIsInsideStorm = false;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		for (TActorIterator<ASupercell_Actor> It(GetWorld()); It; ++It)
		{
			ASupercell_Actor* Storm = *It;
			if (Storm && Storm->StormCore)
			{
				const float Distance = FVector::Dist(
					PlayerPawn->GetActorLocation(),
					Storm->GetActorLocation());

				if (Distance < Storm->StormCore->GetUnscaledSphereRadius())
				{
					bIsInsideStorm = true;
					break;
				}
			}
		}
	}

	// -----------------------------------------------------------------------
	// Update exposure with recovery mechanic
	// -----------------------------------------------------------------------
	CurrentStormExposure = CalculateExposureDelta(
		CurrentStormExposure, bIsInsideStorm, DeltaTime, MaxStormExposure);

	// -----------------------------------------------------------------------
	// Evaluate Day 12 state transition (storm exposure)
	// -----------------------------------------------------------------------
	EOrogenyGameState NewState = EvaluateGameState(
		CurrentSurvivalTime, RequiredSurvivalTime,
		CurrentStormExposure, MaxStormExposure);

	// -----------------------------------------------------------------------
	// Sprint 5: Evaluate Deep Time state (centuries + ecosystem health)
	// -----------------------------------------------------------------------
	UDeepTimeSubsystem* DeepTime = GetWorld()->GetSubsystem<UDeepTimeSubsystem>();
	if (DeepTime)
	{
		CurrentCenturies = static_cast<float>(DeepTime->CurrentDay / 36525.0);
	}

	if (PlayerPawn)
	{
		UEcosystemArmorComponent* Ecosystem = PlayerPawn->FindComponentByClass<UEcosystemArmorComponent>();
		if (Ecosystem)
		{
			CurrentEcosystemHealth = Ecosystem->EcosystemHealth;
		}
	}

	const EOrogenyGameState DeepTimeState = EvaluateDeepTimeGameState(
		CurrentCenturies, TargetSurvivalCenturies,
		CurrentEcosystemHealth, CriticalHealthThreshold);

	// Deep Time defeat/victory overrides Day 12 if still Playing
	if (NewState == EOrogenyGameState::Playing && DeepTimeState != EOrogenyGameState::Playing)
	{
		NewState = DeepTimeState;
	}
	// If Day 12 already triggered defeat, it stands

	if (NewState != CurrentGameState)
	{
		CurrentGameState = NewState;

		if (CurrentGameState == EOrogenyGameState::Victory)
		{
			HandleVictory();
		}
		else if (CurrentGameState == EOrogenyGameState::Defeat)
		{
			HandleDefeat();
		}
	}
}

// ============================================================================
// Win/Loss Handlers
// ============================================================================

void AOrogenyGameModeBase::HandleVictory()
{
	UE_LOG(LogOrogeny, Warning,
		TEXT("=== VICTORY === Survived %.1f seconds. The Mountain endures."),
		CurrentSurvivalTime);

	// Disable player input
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->DisableInput(PC);
	}

	// Fire Blueprint event for UI (fade-out, music, etc.)
	OnVictory();
}

void AOrogenyGameModeBase::HandleDefeat()
{
	UE_LOG(LogOrogeny, Warning,
		TEXT("=== DEFEAT === Storm exposure reached %.1f / %.1f seconds. The storm claims all."),
		CurrentStormExposure, MaxStormExposure);

	// Disable player input
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->DisableInput(PC);
	}

	// Fire Blueprint event for UI (fade-out, tragic music, etc.)
	OnDefeat();
}
