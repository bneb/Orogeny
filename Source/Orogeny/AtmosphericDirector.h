// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AtmosphericDirector.generated.h"

class ASkyAtmosphere;
class AExponentialHeightFog;

/**
 * AAtmosphericDirector
 *
 * Sprint 8: Runtime Atmospheric Control — The World's Breath
 *
 * Hijacks ASkyAtmosphere and AExponentialHeightFog at runtime,
 * driving their properties from UDeepTimeSubsystem and Blight state.
 *
 * ARCHITECTURE:
 *   - BeginPlay: Finds Sky and Fog actors via GetAllActorsOfClass.
 *   - Tick: Reads CurrentSunAngle from DeepTime, calculates
 *     time-of-day alpha and blight ratio, applies to fog density
 *     and sky scattering color via FAtmosphericMath.
 *
 * ROMANTIC PALETTE:
 *   Healthy sky: deep blue/grey (0.1, 0.2, 0.4)
 *   Blighted sky: industrial amber (0.6, 0.3, 0.0)
 *   The shift is continuous and oppressive — the player FEELS
 *   the world dying through the color of the sky.
 */
UCLASS()
class OROGENY_API AAtmosphericDirector : public AActor
{
	GENERATED_BODY()

public:
	AAtmosphericDirector();

	// -----------------------------------------------------------------------
	// Cached Actor References
	// -----------------------------------------------------------------------

	/** Cached ASkyAtmosphere found in the level */
	UPROPERTY(Transient)
	TObjectPtr<AActor> SkyAtmosphereActor;

	/** Cached AExponentialHeightFog found in the level */
	UPROPERTY(Transient)
	TObjectPtr<AActor> HeightFogActor;

	// -----------------------------------------------------------------------
	// Configuration
	// -----------------------------------------------------------------------

	/** Base fog density in clean air */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere")
	float BaseFogDensity = 0.02f;

	/** Romantic healthy sky color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere")
	FLinearColor HealthySkyColor = FLinearColor(0.1f, 0.2f, 0.4f);

	/** Industrial blighted sky color */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere")
	FLinearColor BlightedSkyColor = FLinearColor(0.6f, 0.3f, 0.0f);

	/**
	 * Debug blight ratio override [0, 1].
	 * Set to >= 0 to override the automatic blight query.
	 * Set to -1 to use the automatic system (future integration).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Atmosphere|Debug")
	float DebugBlightRatio = -1.0f;

	// -----------------------------------------------------------------------
	// State (read-only, for debugging)
	// -----------------------------------------------------------------------

	/** Current time-of-day alpha [0, 1] */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Atmosphere|State")
	float CurrentTimeOfDayAlpha = 0.0f;

	/** Current effective blight ratio [0, 1] */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Atmosphere|State")
	float CurrentBlightRatio = 0.0f;

	/** Current fog density being applied */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orogeny|Atmosphere|State")
	float CurrentFogDensity = 0.02f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	/** Resolve the blight ratio (debug override or future system query) */
	float ResolveBlightRatio() const;
};
