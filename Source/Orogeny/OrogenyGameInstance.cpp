// Copyright Orogeny. All Rights Reserved.

#include "OrogenyGameInstance.h"
#include "Orogeny.h"

// ============================================================================
// Init — Intercept Engine Startup
// ============================================================================

void UOrogenyGameInstance::Init()
{
	Super::Init();

	ApplyOptimizedScalability();

	UE_LOG(LogOrogeny, Log,
		TEXT("UOrogenyGameInstance::Init — iGPU optimization profile applied."));
}

// ============================================================================
// Apply Optimized Scalability — CVar Enforcement
// ============================================================================
// Uses IConsoleManager for deterministic, testable CVar setting.
// GEngine->Exec can fail silently in headless/test contexts.
// IConsoleManager::FindConsoleVariable()->Set() is the reliable path.
// ============================================================================

void UOrogenyGameInstance::ApplyOptimizedScalability()
{
	IConsoleManager& Console = IConsoleManager::Get();

	// -----------------------------------------------------------------------
	// Scalability Group CVars
	// -----------------------------------------------------------------------

	auto SetCVar = [&Console](const TCHAR* Name, int32 Value)
	{
		IConsoleVariable* CVar = Console.FindConsoleVariable(Name);
		if (CVar)
		{
			CVar->Set(Value, ECVF_SetByCode);
			UE_LOG(LogOrogeny, Log, TEXT("  CVar %s = %d"), Name, Value);
		}
		else
		{
			UE_LOG(LogOrogeny, Warning, TEXT("  CVar %s NOT FOUND"), Name);
		}
	};

	UE_LOG(LogOrogeny, Log, TEXT("Applying iGPU Optimization Profile (AMD Radeon 780M):"));

	// TSR upscale from 70% native
	SetCVar(TEXT("sg.ResolutionQuality"), TARGET_RESOLUTION_QUALITY);

	// High quality — preserves Romantic aesthetic, skips Epic cinematic overhead
	SetCVar(TEXT("sg.PostProcessQuality"), TARGET_POSTPROCESS_QUALITY);

	// High VSM, no cinematic cascades
	SetCVar(TEXT("sg.ShadowQuality"), TARGET_SHADOW_QUALITY);

	// High Software Lumen, saves radiosity calc time
	SetCVar(TEXT("sg.GlobalIlluminationQuality"), TARGET_GI_QUALITY);

	// Foliage & effects at High
	SetCVar(TEXT("sg.FoliageQuality"), TARGET_FOLIAGE_QUALITY);
	SetCVar(TEXT("sg.EffectsQuality"), TARGET_EFFECTS_QUALITY);

	// Texture streaming enabled
	SetCVar(TEXT("r.TextureStreaming"), TARGET_TEXTURE_STREAMING);

	// Cap texture pool to 3GB (780M shares system RAM)
	SetCVar(TEXT("r.Streaming.PoolSize"), TARGET_STREAMING_POOL_SIZE);

	// Crucial VRAM saver — cap shadow resolution
	SetCVar(TEXT("r.Shadow.MaxResolution"), TARGET_SHADOW_MAX_RESOLUTION);

	UE_LOG(LogOrogeny, Log, TEXT("iGPU Optimization Profile complete."));
}
