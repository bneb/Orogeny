// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TectonicTrenchComponent.generated.h"

class UTextureRenderTarget2D;

/**
 * UTectonicTrenchComponent
 *
 * Day 6: Terrain Deformation Scaffold
 *
 * Manages a persistent R16f Render Target that tracks the Titan's world
 * position and converts it to UV space for terrain deformation painting.
 *
 * ARCHITECTURE:
 * - C++ owns the Render Target lifecycle, format, and UV math.
 * - Blueprint reads TrenchMaskRT + CurrentTrenchUV to paint depth via UCanvas.
 * - Landscape Material reads TrenchMaskRT and applies it to World Position Offset.
 *
 * UV MAPPING:
 * World center (0,0,0) maps to UV center (0.5, 0.5).
 * UV = (WorldPos.XY / WorldSize) + 0.5, clamped to [0.0, 1.0].
 *
 * OPTIMIZATION:
 * - RTF_R16f format — single 16-bit channel, minimal VRAM on the 780M iGPU.
 * - TickComponent early-outs when velocity == 0 (stationary Titan).
 */
UCLASS(ClassGroup=(Orogeny), meta=(BlueprintSpawnableComponent))
class OROGENY_API UTectonicTrenchComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTectonicTrenchComponent();

	// -----------------------------------------------------------------------
	// Design Constants — exposed for TDD verification
	// -----------------------------------------------------------------------

	/** Physical size of the world region this texture covers (cm) */
	static constexpr float DEFAULT_WORLD_SIZE = 100000.0f;

	/** Render target resolution (pixels per axis) */
	static constexpr int32 DEFAULT_RT_RESOLUTION = 2048;

	// -----------------------------------------------------------------------
	// Configuration Properties
	// -----------------------------------------------------------------------

	/** Physical size of the map this texture represents (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Trench",
		meta = (ClampMin = "1.0"))
	float WorldSize = DEFAULT_WORLD_SIZE;

	/** Resolution of the trench mask render target (per axis). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orogeny|Trench",
		meta = (ClampMin = "64", ClampMax = "8192"))
	int32 RenderTargetResolution = DEFAULT_RT_RESOLUTION;

	// -----------------------------------------------------------------------
	// Runtime State
	// -----------------------------------------------------------------------

	/** The persistent render target for terrain deformation depth mask. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Orogeny|Trench")
	TObjectPtr<UTextureRenderTarget2D> TrenchMaskRT;

	/** The current UV coordinate where the Titan stands. Updated each tick. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Orogeny|Trench")
	FVector2D CurrentTrenchUV = FVector2D(0.5, 0.5);

	/** True if the Titan is moving and the mask should be painted this frame. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Orogeny|Trench")
	bool bShouldPaintThisFrame = false;

	// -----------------------------------------------------------------------
	// Pure Math — World to UV Mapping
	// -----------------------------------------------------------------------

	/**
	 * Convert a world-space location to UV space on the trench mask.
	 *
	 * World center (0,0,0) → UV (0.5, 0.5).
	 * Result is clamped to [0.0, 1.0] on both axes.
	 * Z component is ignored (terrain is 2D projection).
	 *
	 * This is a static pure function so TDD can call it without a UWorld.
	 *
	 * @param WorldLocation     The world-space position to convert.
	 * @param InWorldSize       The physical size of the mapped region.
	 * @return                  UV coordinate in [0,1] range.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Orogeny|Trench")
	static FVector2D GetUVFromWorldLocation(FVector WorldLocation, float InWorldSize);

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
};
