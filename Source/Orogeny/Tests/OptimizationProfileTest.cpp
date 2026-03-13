// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: iGPU Optimization Profile Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify that CVar locks are enforced after ApplyOptimizedScalability().
// 2. If ANY CVar drifts from the target, iGPU frame budget is broken.
// 3. Uses IConsoleManager for deterministic reads — no GEngine dependency.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Optimization"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "OrogenyGameInstance.h"
#include "HAL/IConsoleManager.h"

// Helper: read a CVar's int value
static int32 GetCVarInt(const TCHAR* Name)
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name);
	return CVar ? CVar->GetInt() : -1;
}

// ============================================================================
// Test: Resolution Quality Lock (sg.ResolutionQuality = 70)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOptResolutionTest,
	"Orogeny.Optimization.ResolutionQuality",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FOptResolutionTest::RunTest(const FString& Parameters)
{
	UOrogenyGameInstance* GI = NewObject<UOrogenyGameInstance>();
	GI->ApplyOptimizedScalability();

	const int32 Value = GetCVarInt(TEXT("sg.ResolutionQuality"));
	TestEqual(TEXT("sg.ResolutionQuality must be 70"),
		Value, UOrogenyGameInstance::TARGET_RESOLUTION_QUALITY);
	return true;
}

// ============================================================================
// Test: GI Quality Lock (sg.GlobalIlluminationQuality = 2)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOptGIQualityTest,
	"Orogeny.Optimization.GIQuality",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FOptGIQualityTest::RunTest(const FString& Parameters)
{
	UOrogenyGameInstance* GI = NewObject<UOrogenyGameInstance>();
	GI->ApplyOptimizedScalability();

	const int32 Value = GetCVarInt(TEXT("sg.GlobalIlluminationQuality"));
	TestEqual(TEXT("sg.GlobalIlluminationQuality must be 2"),
		Value, UOrogenyGameInstance::TARGET_GI_QUALITY);
	return true;
}

// ============================================================================
// Test: Streaming Pool Lock (r.Streaming.PoolSize = 3072)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOptStreamingPoolTest,
	"Orogeny.Optimization.StreamingPoolSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FOptStreamingPoolTest::RunTest(const FString& Parameters)
{
	UOrogenyGameInstance* GI = NewObject<UOrogenyGameInstance>();
	GI->ApplyOptimizedScalability();

	const int32 Value = GetCVarInt(TEXT("r.Streaming.PoolSize"));
	TestEqual(TEXT("r.Streaming.PoolSize must be 3072"),
		Value, UOrogenyGameInstance::TARGET_STREAMING_POOL_SIZE);
	return true;
}

// ============================================================================
// Test: Shadow Max Resolution Lock (r.Shadow.MaxResolution = 1024)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOptShadowResTest,
	"Orogeny.Optimization.ShadowMaxResolution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FOptShadowResTest::RunTest(const FString& Parameters)
{
	UOrogenyGameInstance* GI = NewObject<UOrogenyGameInstance>();
	GI->ApplyOptimizedScalability();

	const int32 Value = GetCVarInt(TEXT("r.Shadow.MaxResolution"));
	TestEqual(TEXT("r.Shadow.MaxResolution must be 1024"),
		Value, UOrogenyGameInstance::TARGET_SHADOW_MAX_RESOLUTION);
	return true;
}
