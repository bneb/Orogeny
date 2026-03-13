// Copyright Orogeny. All Rights Reserved.

#include "MetaSoundDirector.h"
#include "Orogeny.h"
#include "MetaSoundMath.h"
#include "DeepTimeSubsystem.h"
#include "EcosystemArmorComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// ============================================================================
// Constructor
// ============================================================================

AMetaSoundDirector::AMetaSoundDirector()
{
	PrimaryActorTick.bCanEverTick = true;

	// Audio parameter updates are smooth — 10Hz is plenty
	PrimaryActorTick.TickInterval = 0.1f;

	// Create audio component
	MainAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MainAudio"));
	RootComponent = MainAudioComponent;

	// Auto-activate so it starts playing when BeginPlay fires
	MainAudioComponent->bAutoActivate = true;
}

// ============================================================================
// BeginPlay
// ============================================================================

void AMetaSoundDirector::BeginPlay()
{
	Super::BeginPlay();

	if (MainAudioComponent && MainAudioComponent->Sound)
	{
		MainAudioComponent->Play();
		UE_LOG(LogOrogeny, Log,
			TEXT("MetaSoundDirector: Playing %s"),
			*MainAudioComponent->Sound->GetName());
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("MetaSoundDirector: No Sound asset assigned to MainAudioComponent!"));
		UE_LOG(LogOrogeny, Warning,
			TEXT("  Assign a MetaSoundSource in the Details panel."));
	}
}

// ============================================================================
// Tick — Read game state → Calculate → Send to MetaSound
// ============================================================================

void AMetaSoundDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MainAudioComponent)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// -----------------------------------------------------------------------
	// Time Compression Pitch
	// -----------------------------------------------------------------------
	float TimeScale = 1.0f;
	if (UDeepTimeSubsystem* DeepTime = World->GetSubsystem<UDeepTimeSubsystem>())
	{
		TimeScale = DeepTime->CurrentTimeScale;
	}
	CurrentTimePitch = FMetaSoundMath::CalculateTimeCompressionPitch(TimeScale);
	MainAudioComponent->SetFloatParameter(ParamTimePitch, CurrentTimePitch);

	// -----------------------------------------------------------------------
	// Wind Intensity (from player altitude)
	// -----------------------------------------------------------------------
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn)
	{
		const float PlayerZ = PlayerPawn->GetActorLocation().Z;
		CurrentWindIntensity = FMetaSoundMath::CalculateWindIntensity(PlayerZ);
		MainAudioComponent->SetFloatParameter(ParamWindIntensity, CurrentWindIntensity);

		// -------------------------------------------------------------------
		// Fauna Volume (from ecosystem health)
		// -------------------------------------------------------------------
		UEcosystemArmorComponent* Ecosystem =
			PlayerPawn->FindComponentByClass<UEcosystemArmorComponent>();
		if (Ecosystem)
		{
			CurrentFaunaVolume = FMetaSoundMath::CalculateFaunaVolume(
				Ecosystem->EcosystemHealth);
			MainAudioComponent->SetFloatParameter(ParamFaunaVolume, CurrentFaunaVolume);
		}
	}
}
