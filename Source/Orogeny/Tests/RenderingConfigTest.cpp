// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Rendering Configuration Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. These tests verify that DefaultEngine.ini CVars are correctly loaded
//    and enforcing our AMD Radeon 780M iGPU performance constraints.
// 2. Tests are written FIRST (Red) — before any visual maps exist.
// 3. When the project compiles and these tests pass (Green), the rendering
//    pipeline is locked and verified.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Config"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "HAL/IConsoleManager.h"

// ============================================================================
// Test: Rendering Pipeline Configuration
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRenderingConfigTest,
	"Orogeny.Config.RenderingTargets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FRenderingConfigTest::RunTest(const FString& Parameters)
{
	// -----------------------------------------------------------------------
	// Test 1: Lumen Hardware RayTracing MUST be disabled
	// Rationale: HW RT will annihilate the 780M's frame budget. We use
	// Software Lumen exclusively for GI on this iGPU.
	// -----------------------------------------------------------------------
	{
		static const auto CVarLumenHW = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.Lumen.HardwareRayTracing"));
		if (CVarLumenHW)
		{
			TestTrue(
				TEXT("[CRITICAL] Lumen Hardware RayTracing must be disabled (0) for iGPU performance"),
				CVarLumenHW->GetValueOnGameThread() == 0
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.Lumen.HardwareRayTracing — Engine version mismatch?"));
		}
	}

	// -----------------------------------------------------------------------
	// Test 2: Volumetric Fog Grid Pixel Size MUST be 16
	// Rationale: GridPixelSize=16 balances volumetric density against
	// fill-rate cost. Lower values (8) tank performance on integrated GPUs.
	// -----------------------------------------------------------------------
	{
		static const auto CVarFogGrid = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.VolumetricFog.GridPixelSize"));
		if (CVarFogGrid)
		{
			TestEqual(
				TEXT("Volumetric Fog Grid Pixel Size must be 16 for iGPU optimization"),
				CVarFogGrid->GetValueOnGameThread(), 16
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.VolumetricFog.GridPixelSize"));
		}
	}

	// -----------------------------------------------------------------------
	// Test 3: Dynamic GI Method MUST be Software Lumen (1)
	// Rationale: Method 1 = Lumen. Method 0 = None. Method 2 = SSGI.
	// Software Lumen gives us beautiful GI within our frame budget.
	// -----------------------------------------------------------------------
	{
		static const auto CVarGI = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.DynamicGlobalIlluminationMethod"));
		if (CVarGI)
		{
			TestEqual(
				TEXT("Dynamic GI Method must be Lumen (1)"),
				CVarGI->GetValueOnGameThread(), 1
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.DynamicGlobalIlluminationMethod"));
		}
	}

	// -----------------------------------------------------------------------
	// Test 4: Reflection Method MUST be Lumen Reflections (1)
	// Rationale: Lumen reflections provide physically-based reflections
	// without the cost of hardware RT or SSR artifacts.
	// -----------------------------------------------------------------------
	{
		static const auto CVarReflection = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.ReflectionMethod"));
		if (CVarReflection)
		{
			TestEqual(
				TEXT("Reflection Method must be Lumen (1)"),
				CVarReflection->GetValueOnGameThread(), 1
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.ReflectionMethod"));
		}
	}

	// -----------------------------------------------------------------------
	// Test 5: Anti-Aliasing MUST be TSR (4)
	// Rationale: TSR provides reconstruction-based upscaling. Critical for
	// rendering at lower internal resolution while maintaining visual fidelity.
	// -----------------------------------------------------------------------
	{
		static const auto CVarAA = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.AntiAliasingMethod"));
		if (CVarAA)
		{
			TestEqual(
				TEXT("Anti-Aliasing Method must be TSR (4)"),
				CVarAA->GetValueOnGameThread(), 4
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.AntiAliasingMethod"));
		}
	}

	// -----------------------------------------------------------------------
	// Test 6: Virtual Shadow Maps MUST be enabled (1)
	// Rationale: VSM gives per-light shadow quality without the VRAM overhead
	// of traditional cascaded shadow maps on our shared-memory iGPU.
	// -----------------------------------------------------------------------
	{
		static const auto CVarVSM = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.Shadow.Virtual.Enable"));
		if (CVarVSM)
		{
			TestEqual(
				TEXT("Virtual Shadow Maps must be enabled (1)"),
				CVarVSM->GetValueOnGameThread(), 1
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.Shadow.Virtual.Enable"));
		}
	}

	return true;
}

// ============================================================================
// Test: Volumetric Fog Sub-configuration
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVolumetricFogConfigTest,
	"Orogeny.Config.VolumetricFog",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVolumetricFogConfigTest::RunTest(const FString& Parameters)
{
	// Test: Volumetric Fog must be globally enabled
	{
		static const auto CVarFogEnabled = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.VolumetricFog"));
		if (CVarFogEnabled)
		{
			TestTrue(
				TEXT("Volumetric Fog must be globally enabled"),
				CVarFogEnabled->GetValueOnGameThread() == 1
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.VolumetricFog"));
		}
	}

	// Test: Grid Z slices must be 64
	{
		static const auto CVarFogZ = IConsoleManager::Get().FindTConsoleVariableDataInt(
			TEXT("r.VolumetricFog.GridSizeZ"));
		if (CVarFogZ)
		{
			TestEqual(
				TEXT("Volumetric Fog Grid Z slices must be 64"),
				CVarFogZ->GetValueOnGameThread(), 64
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.VolumetricFog.GridSizeZ"));
		}
	}

	return true;
}

// ============================================================================
// Test: Lumen Radiosity Optimization
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FLumenRadiosityConfigTest,
	"Orogeny.Config.LumenRadiosity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FLumenRadiosityConfigTest::RunTest(const FString& Parameters)
{
	// Test: Radiosity update factor should be halved (0.5) for iGPU
	{
		static const auto CVarRadiosity = IConsoleManager::Get().FindTConsoleVariableDataFloat(
			TEXT("r.LumenScene.Radiosity.UpdateFactor"));
		if (CVarRadiosity)
		{
			TestEqual(
				TEXT("Lumen Radiosity Update Factor must be 0.5 for iGPU optimization"),
				CVarRadiosity->GetValueOnGameThread(), 0.5f
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.LumenScene.Radiosity.UpdateFactor"));
		}
	}

	return true;
}
