// Copyright Orogeny. All Rights Reserved.

#include "TectonicAudioComponent.h"
#include "Orogeny.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// ============================================================================
// Constructor
// ============================================================================

UTectonicAudioComponent::UTectonicAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	UE_LOG(LogOrogeny, Log,
		TEXT("UTectonicAudioComponent constructed: SpeedParam=%s"),
		*SpeedParameterName.ToString());
}

// ============================================================================
// BeginPlay — Spawn the Movement Loop Audio
// ============================================================================

void UTectonicAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	if (MovementLoopSound && GetOwner())
	{
		MovementAudioComp = UGameplayStatics::SpawnSoundAttached(
			MovementLoopSound,
			GetOwner()->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			EAttachLocation::KeepRelativeOffset,
			true,        // bStopWhenAttachedToDestroyed
			1.0f,        // VolumeMultiplier
			1.0f,        // PitchMultiplier
			0.0f,        // StartTime
			nullptr,     // AttenuationSettings
			nullptr,     // ConcurrencySettings
			false        // bAutoDestroy — we manage lifecycle
		);

		if (MovementAudioComp)
		{
			// Don't auto-play — we start/stop based on velocity
			MovementAudioComp->Stop();

			UE_LOG(LogOrogeny, Log,
				TEXT("Movement loop audio spawned and attached (stopped, awaiting velocity)."));
		}
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("MovementLoopSound not assigned — no tectonic audio will play. "
			     "Assign a MetaSound in Blueprint/Editor."));
	}
}

// ============================================================================
// Speed Alpha — The Critical Math
// ============================================================================
// Simple normalization, but it MUST be clamped [0, 1].
// Any value outside this range would blow out the MetaSound graph
// (LFO frequency, volume multipliers, etc.).
// ============================================================================

float UTectonicAudioComponent::CalculateSpeedAlpha(
	float CurrentSpeed, float MaxSpeed)
{
	if (MaxSpeed <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f);
}

// ============================================================================
// TickComponent — Push Alpha to MetaSound + Manage Playback
// ============================================================================

void UTectonicAudioComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// -----------------------------------------------------------------------
	// Read velocity from the owning character
	// -----------------------------------------------------------------------
	float CurrentSpeed = 0.0f;
	float MaxSpeed = 150.0f;

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (Character && Character->GetCharacterMovement())
	{
		CurrentSpeed = Character->GetCharacterMovement()->Velocity.Size();
		MaxSpeed = Character->GetCharacterMovement()->GetMaxSpeed();
	}

	// -----------------------------------------------------------------------
	// Calculate speed alpha
	// -----------------------------------------------------------------------
	CurrentSpeedAlpha = CalculateSpeedAlpha(CurrentSpeed, MaxSpeed);

	// -----------------------------------------------------------------------
	// Push parameter to MetaSound + manage playback state
	// -----------------------------------------------------------------------
	if (MovementAudioComp)
	{
		MovementAudioComp->SetFloatParameter(SpeedParameterName, CurrentSpeedAlpha);

		if (CurrentSpeedAlpha > SILENCE_THRESHOLD && !MovementAudioComp->IsPlaying())
		{
			MovementAudioComp->Play();
		}
		else if (CurrentSpeedAlpha <= SILENCE_THRESHOLD && MovementAudioComp->IsPlaying())
		{
			MovementAudioComp->Stop();
		}
	}
}
