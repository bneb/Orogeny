// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MetaSoundDirector.generated.h"

class UAudioComponent;

/**
 * AMetaSoundDirector
 *
 * Sprint 10: Runtime Audio Parameter Driver — The World's Voice
 *
 * Reads game state each tick and feeds normalized float parameters
 * into the MetaSound graph via UAudioComponent::SetFloatParameter().
 *
 * ARCHITECTURE:
 *   - MainAudioComponent hosts the MetaSoundSource asset.
 *   - FMetaSoundMath translates raw game state to [0, 1] parameters.
 *   - MetaSound graph maps parameters to DSP (wind, fauna, pitch).
 *
 * PARAMETER NAMES must match MetaSound graph inputs exactly:
 *   "WindIntensity", "FaunaVolume", "TimePitch"
 */
UCLASS()
class OROGENY_API AMetaSoundDirector : public AActor
{
	GENERATED_BODY()

public:
	AMetaSoundDirector();

	// -----------------------------------------------------------------------
	// Components
	// -----------------------------------------------------------------------

	/** The audio component hosting the MetaSoundSource patch */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Audio")
	TObjectPtr<UAudioComponent> MainAudioComponent;

	// -----------------------------------------------------------------------
	// Parameter Names (must match MetaSound graph inputs)
	// -----------------------------------------------------------------------

	/** MetaSound input: wind howl intensity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Audio")
	FName ParamWindIntensity = "WindIntensity";

	/** MetaSound input: fauna chorus volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Audio")
	FName ParamFaunaVolume = "FaunaVolume";

	/** MetaSound input: time compression pitch */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Audio")
	FName ParamTimePitch = "TimePitch";

	// -----------------------------------------------------------------------
	// State (read-only, for debugging)
	// -----------------------------------------------------------------------

	/** Current wind intensity being sent to MetaSound */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Audio|State")
	float CurrentWindIntensity = 0.0f;

	/** Current fauna volume being sent to MetaSound */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Audio|State")
	float CurrentFaunaVolume = 0.0f;

	/** Current time compression pitch being sent to MetaSound */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Audio|State")
	float CurrentTimePitch = 1.0f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
