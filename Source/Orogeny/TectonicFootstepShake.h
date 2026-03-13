// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "TectonicFootstepShake.generated.h"

/**
 * UTectonicFootstepShake
 *
 * Camera shake triggered by each Titan footfall. Uses Perlin noise to
 * simulate a deep, rolling earthquake rather than a high-frequency rattle.
 *
 * DESIGN INTENT:
 *   - Short duration (0.5s) — one shake per footstep, not continuous
 *   - Z-dominant amplitude (15 units) — the ground heaves vertically
 *   - Low pitch rotation (2°) — subtle forward tilt, not nausea-inducing
 *   - 10 Hz frequency — sub-bass rumble, matches MetaSounds footstep
 *
 * USAGE:
 *   Triggered by UAnimNotify_TectonicStep when placed on walk/run montages.
 *   The animation team controls WHEN it fires; this class controls HOW it feels.
 *
 * TDD: Pattern type and duration verified in CameraScaleTest.cpp
 */
UCLASS()
class OROGENY_API UTectonicFootstepShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UTectonicFootstepShake(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// -----------------------------------------------------------------------
	// Design Constants — exposed for TDD verification
	// -----------------------------------------------------------------------

	/** Shake duration in seconds (one footfall = one shake cycle) */
	static constexpr float SHAKE_DURATION = 0.5f;

	/** Z-axis location amplitude — vertical ground heave */
	static constexpr float LOCATION_AMPLITUDE_Z = 15.0f;

	/** Pitch rotation amplitude in degrees — subtle forward tilt */
	static constexpr float ROTATION_AMPLITUDE_PITCH = 2.0f;

	/** Perlin noise frequency in Hz — deep rolling rumble */
	static constexpr float NOISE_FREQUENCY = 10.0f;
};
