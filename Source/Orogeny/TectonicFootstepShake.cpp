// Copyright Orogeny. All Rights Reserved.

#include "TectonicFootstepShake.h"
#include "Orogeny.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"

// ============================================================================
// Constructor — Configure the Perlin Noise Camera Shake
// ============================================================================

UTectonicFootstepShake::UTectonicFootstepShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create the Perlin noise pattern as the root shake pattern
	UPerlinNoiseCameraShakePattern* NoisePattern = CreateDefaultSubobject<UPerlinNoiseCameraShakePattern>(TEXT("TectonicNoisePattern"));

	// -----------------------------------------------------------------------
	// Timing
	// -----------------------------------------------------------------------
	NoisePattern->Duration = SHAKE_DURATION;

	// -----------------------------------------------------------------------
	// Location — Vertical heave dominates
	// -----------------------------------------------------------------------
	// X (forward/back): minimal — the ground doesn't slide
	NoisePattern->LocationAmplitudeMultiplier = 1.0f;
	NoisePattern->X.Amplitude = 0.0f;
	NoisePattern->X.Frequency = NOISE_FREQUENCY;

	// Y (left/right): minimal — no lateral drift
	NoisePattern->Y.Amplitude = 0.0f;
	NoisePattern->Y.Frequency = NOISE_FREQUENCY;

	// Z (up/down): PRIMARY — the earth heaves beneath the camera
	NoisePattern->Z.Amplitude = LOCATION_AMPLITUDE_Z;
	NoisePattern->Z.Frequency = NOISE_FREQUENCY;

	// -----------------------------------------------------------------------
	// Rotation — Subtle pitch to sell the impact
	// -----------------------------------------------------------------------
	NoisePattern->RotationAmplitudeMultiplier = 1.0f;

	// Pitch: slight forward tilt — the camera nods with the quake
	NoisePattern->Pitch.Amplitude = ROTATION_AMPLITUDE_PITCH;
	NoisePattern->Pitch.Frequency = NOISE_FREQUENCY;

	// Yaw: none — no horizontal wobble
	NoisePattern->Yaw.Amplitude = 0.0f;
	NoisePattern->Yaw.Frequency = NOISE_FREQUENCY;

	// Roll: none — no barrel roll
	NoisePattern->Roll.Amplitude = 0.0f;
	NoisePattern->Roll.Frequency = NOISE_FREQUENCY;

	// -----------------------------------------------------------------------
	// Assign as root pattern
	// -----------------------------------------------------------------------
	SetRootShakePattern(NoisePattern);

	UE_LOG(LogOrogeny, Log, TEXT("UTectonicFootstepShake configured: Duration=%.1fs, Z=%.0f, Pitch=%.1f°, Freq=%.0fHz"),
		SHAKE_DURATION, LOCATION_AMPLITUDE_Z, ROTATION_AMPLITUDE_PITCH, NOISE_FREQUENCY);
}
