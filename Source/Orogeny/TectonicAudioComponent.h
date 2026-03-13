// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TectonicAudioComponent.generated.h"

class USoundBase;
class UAudioComponent;

/**
 * UTectonicAudioComponent
 *
 * Day 10: Tectonic Audio Data Pipeline
 *
 * Maps the Titan's velocity to audio parameters that feed MetaSound graphs.
 * Drives sub-bass rumble, grinding foley, and dynamic volume/pitch
 * scaled to the physics data locked in on Day 3.
 *
 * ARCHITECTURE:
 * - CalculateSpeedAlpha is static & pure — TDD without a UWorld.
 * - TickComponent reads velocity, pushes normalized alpha to audio params.
 * - Manages playback state: Play when moving, Stop when stationary.
 * - SpeedAlpha [0,1] drives MetaSound graph inputs.
 *
 * MATH:
 *   SpeedAlpha = Clamp(CurrentSpeed / MaxSpeed, 0, 1)
 *
 * AUDIO BUDGET:
 *   Single UAudioComponent with one MetaSound source — minimal CPU.
 *   No per-frame audio spawning. Parameter-driven DSP only.
 */
UCLASS(ClassGroup=(Orogeny), meta=(BlueprintSpawnableComponent))
class OROGENY_API UTectonicAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTectonicAudioComponent();

	// -----------------------------------------------------------------------
	// Design Constants
	// -----------------------------------------------------------------------

	/** Threshold below which audio stops (prevents inaudible playback cost). */
	static constexpr float SILENCE_THRESHOLD = 0.01f;

	// -----------------------------------------------------------------------
	// Configuration Properties
	// -----------------------------------------------------------------------

	/** MetaSound source for the movement loop (sub-bass + grinding foley).
	 *  Assign in Blueprint/Editor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Audio")
	TObjectPtr<USoundBase> MovementLoopSound;

	/** Name of the float parameter on the MetaSound graph that receives
	 *  the normalized speed value [0, 1]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Audio")
	FName SpeedParameterName = "SpeedAlpha";

	// -----------------------------------------------------------------------
	// Runtime State
	// -----------------------------------------------------------------------

	/** The spawned audio component for the movement loop. Created in BeginPlay. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Orogeny|Audio")
	TObjectPtr<UAudioComponent> MovementAudioComp;

	/** Current speed alpha [0, 1]. Updated each tick. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Orogeny|Audio")
	float CurrentSpeedAlpha = 0.0f;

	// -----------------------------------------------------------------------
	// Pure Math — Velocity to Audio Alpha
	// -----------------------------------------------------------------------

	/**
	 * Normalize speed to a [0, 1] alpha for audio parameter driving.
	 *
	 * FORMULA:
	 *   SpeedAlpha = Clamp(CurrentSpeed / MaxSpeed, 0, 1)
	 *
	 * PROOF:
	 *   Speed=0:   Alpha=0.0
	 *   Speed=75:  Alpha=0.5
	 *   Speed=150: Alpha=1.0
	 *   Speed=300: Alpha=1.0 (clamped)
	 *
	 * @param CurrentSpeed   Current character speed.
	 * @param MaxSpeed       The character's max walk speed.
	 * @return               Normalized alpha [0, 1].
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|Audio")
	static float CalculateSpeedAlpha(float CurrentSpeed, float MaxSpeed);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
};
