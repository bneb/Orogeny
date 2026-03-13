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
		IConsoleVariable* CVarLumenHW = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.Lumen.HardwareRayTracing"));
		if (CVarLumenHW)
		{
			TestTrue(
				TEXT("[CRITICAL] Lumen Hardware RayTracing must be disabled (0) for iGPU performance"),
				CVarLumenHW->GetInt() == 0
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
		IConsoleVariable* CVarFogGrid = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.VolumetricFog.GridPixelSize"));
		if (CVarFogGrid)
		{
			TestEqual(
				TEXT("Volumetric Fog Grid Pixel Size must be 16 for iGPU optimization"),
				CVarFogGrid->GetInt(), 16
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
		IConsoleVariable* CVarGI = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.DynamicGlobalIlluminationMethod"));
		if (CVarGI)
		{
			TestEqual(
				TEXT("Dynamic GI Method must be Lumen (1)"),
				CVarGI->GetInt(), 1
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
		IConsoleVariable* CVarReflection = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.ReflectionMethod"));
		if (CVarReflection)
		{
			TestEqual(
				TEXT("Reflection Method must be Lumen (1)"),
				CVarReflection->GetInt(), 1
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
		IConsoleVariable* CVarAA = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.AntiAliasingMethod"));
		if (CVarAA)
		{
			TestEqual(
				TEXT("Anti-Aliasing Method must be TSR (4)"),
				CVarAA->GetInt(), 4
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
		IConsoleVariable* CVarVSM = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.Shadow.Virtual.Enable"));
		if (CVarVSM)
		{
			TestEqual(
				TEXT("Virtual Shadow Maps must be enabled (1)"),
				CVarVSM->GetInt(), 1
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
		IConsoleVariable* CVarFogEnabled = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.VolumetricFog"));
		if (CVarFogEnabled)
		{
			TestTrue(
				TEXT("Volumetric Fog must be globally enabled"),
				CVarFogEnabled->GetInt() == 1
			);
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.VolumetricFog"));
		}
	}

	// Test: Grid Z slices must be 64
	{
		IConsoleVariable* CVarFogZ = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.VolumetricFog.GridSizeZ"));
		if (CVarFogZ)
		{
			TestEqual(
				TEXT("Volumetric Fog Grid Z slices must be 64"),
				CVarFogZ->GetInt(), 64
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
		IConsoleVariable* CVarRadiosity = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.LumenScene.Radiosity.UpdateFactor"));
		if (CVarRadiosity)
		{
			const float Value = CVarRadiosity->GetFloat();

			// Under -nullrhi (headless CI), this CVar may retain its code default (0.0)
			// because the Lumen renderer doesn't fully initialize. Skip gracefully.
			if (FMath::IsNearlyZero(Value) && !FApp::CanEverRender())
			{
				AddInfo(TEXT("Skipping Radiosity test: CVar not initialized under -nullrhi (headless mode)"));
			}
			else
			{
				TestEqual(
					TEXT("Lumen Radiosity Update Factor must be 0.5 for iGPU optimization"),
					Value, 0.5f
				);
			}
		}
		else
		{
			AddError(TEXT("Could not find CVar: r.LumenScene.Radiosity.UpdateFactor"));
		}
	}

	return true;
}
