// Copyright Orogeny. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OrogenyGameInstance.generated.h"

/**
 * UOrogenyGameInstance
 *
 * Day 13: iGPU Optimization Pipeline — AMD Radeon 780M Profile
 *
 * Intercepts the engine's startup sequence and forces a highly specific,
 * optimized CVar configuration tailored for integrated graphics.
 *
 * ARCHITECTURE:
 *   Uses IConsoleManager::FindConsoleVariable()->Set() for deterministic
 *   CVar enforcement — perfectly testable without GEngine->Exec.
 *
 * BUDGET:
 *   Target: 60 FPS (16.6ms frame budget) on AMD Radeon 780M
 *   Shared Memory: 780M uses system RAM, so texture pool is capped
 *   TSR: Upscales from 70% native resolution to save fill-rate
 *
 * SCALABILITY PROFILE:
 *   ResolutionQuality: 70  (TSR upscale)
 *   PostProcess:       2   (High — preserves Romantic aesthetic)
 *   Shadows:           2   (High VSM, no cinematic cascades)
 *   GI:                2   (High Software Lumen)
 *   Foliage:           2
 *   Effects:           2
 *   TextureStreaming:   1   (enabled)
 *   StreamingPool:     3072 MB (cap for shared RAM)
 *   ShadowMaxRes:      1024 (VRAM saver)
 */
UCLASS()
class OROGENY_API UOrogenyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// Design Constants — the 780M Budget
	// -----------------------------------------------------------------------

	static constexpr int32 TARGET_RESOLUTION_QUALITY = 70;
	static constexpr int32 TARGET_POSTPROCESS_QUALITY = 2;
	static constexpr int32 TARGET_SHADOW_QUALITY = 2;
	static constexpr int32 TARGET_GI_QUALITY = 2;
	static constexpr int32 TARGET_FOLIAGE_QUALITY = 2;
	static constexpr int32 TARGET_EFFECTS_QUALITY = 2;
	static constexpr int32 TARGET_TEXTURE_STREAMING = 1;
	static constexpr int32 TARGET_STREAMING_POOL_SIZE = 3072;
	static constexpr int32 TARGET_SHADOW_MAX_RESOLUTION = 1024;

	// -----------------------------------------------------------------------
	// Initialization
	// -----------------------------------------------------------------------

	virtual void Init() override;

	/**
	 * Apply the optimized scalability profile for iGPU.
	 * Uses IConsoleManager for deterministic CVar enforcement.
	 * Separated from Init() for TDD testability.
	 */
	UFUNCTION(BlueprintCallable, Category = "Orogeny|Optimization")
	void ApplyOptimizedScalability();
};
