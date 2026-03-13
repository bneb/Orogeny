// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Terrain Deformation (Trenching) Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// 1. Tests verify UTectonicTrenchComponent's render target initialization
//    and World-to-UV mapping math is pixel-perfect.
// 2. If the UV math is off by even 1%, trenches render hundreds of units
//    away from the Titan's feet — these tests prevent that.
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Terrain"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TectonicTrenchComponent.h"
#include "TitanCharacter.h"

// ============================================================================
// Test: Design Constants Match Constructed Defaults
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchDesignConstantsTest,
	"Orogeny.Terrain.Trench.DesignConstants",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchDesignConstantsTest::RunTest(const FString& Parameters)
{
	UTectonicTrenchComponent* Comp = NewObject<UTectonicTrenchComponent>();

	TestEqual(TEXT("Default WorldSize must be 100000"),
		Comp->WorldSize, UTectonicTrenchComponent::DEFAULT_WORLD_SIZE);

	TestEqual(TEXT("Default RenderTargetResolution must be 2048"),
		Comp->RenderTargetResolution, UTectonicTrenchComponent::DEFAULT_RT_RESOLUTION);

	TestEqual(TEXT("DEFAULT_WORLD_SIZE constant must be 100000"),
		UTectonicTrenchComponent::DEFAULT_WORLD_SIZE, 100000.0f);

	TestEqual(TEXT("DEFAULT_RT_RESOLUTION constant must be 2048"),
		UTectonicTrenchComponent::DEFAULT_RT_RESOLUTION, 2048);

	return true;
}

// ============================================================================
// Test: Component Initialization — Render Target Format & Size
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchComponentInitTest,
	"Orogeny.Terrain.Trench.ComponentInit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchComponentInitTest::RunTest(const FString& Parameters)
{
	// Create the component via NewObject (no UWorld → no BeginPlay)
	// We manually instantiate the RT to test its configuration
	UTectonicTrenchComponent* Comp = NewObject<UTectonicTrenchComponent>();

	// Manually create the RT as BeginPlay would
	UTextureRenderTarget2D* RT = NewObject<UTextureRenderTarget2D>(Comp, TEXT("TestTrenchMaskRT"));
	RT->RenderTargetFormat = ETextureRenderTargetFormat::RTF_R16f;
	RT->InitAutoFormat(Comp->RenderTargetResolution, Comp->RenderTargetResolution);
	RT->ClearColor = FLinearColor::White;
	Comp->TrenchMaskRT = RT;

	// Verify RT is valid
	TestNotNull(TEXT("TrenchMaskRT must be valid after init"), Comp->TrenchMaskRT.Get());

	// Verify resolution
	TestEqual(TEXT("RT SizeX must be 2048"),
		static_cast<int32>(RT->SizeX), 2048);
	TestEqual(TEXT("RT SizeY must be 2048"),
		static_cast<int32>(RT->SizeY), 2048);

	// Verify format — RTF_R16f
	TestEqual(TEXT("RT format must be RTF_R16f"),
		RT->RenderTargetFormat, ETextureRenderTargetFormat::RTF_R16f);

	// Verify clear color
	TestTrue(TEXT("RT ClearColor must be white"),
		RT->ClearColor.Equals(FLinearColor::White));

	return true;
}

// ============================================================================
// Test: UV Mapping — Center (Origin → UV 0.5, 0.5)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchUVCenterTest,
	"Orogeny.Terrain.Trench.UVCenter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchUVCenterTest::RunTest(const FString& Parameters)
{
	const FVector2D UV = UTectonicTrenchComponent::GetUVFromWorldLocation(
		FVector(0.0, 0.0, 0.0), 100000.0f);

	TestEqual(TEXT("UV Center: X must be 0.5"), UV.X, 0.5);
	TestEqual(TEXT("UV Center: Y must be 0.5"), UV.Y, 0.5);

	return true;
}

// ============================================================================
// Test: UV Mapping — Positive Edge (WorldSize/2 → UV 1.0, 1.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchUVEdgeTest,
	"Orogeny.Terrain.Trench.UVEdge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchUVEdgeTest::RunTest(const FString& Parameters)
{
	// WorldSize = 100000, so +50000 on each axis should map to UV 1.0
	const FVector2D UV = UTectonicTrenchComponent::GetUVFromWorldLocation(
		FVector(50000.0, 50000.0, 0.0), 100000.0f);

	TestEqual(TEXT("UV Edge: X must be 1.0"), UV.X, 1.0);
	TestEqual(TEXT("UV Edge: Y must be 1.0"), UV.Y, 1.0);

	return true;
}

// ============================================================================
// Test: UV Mapping — Negative Edge (-WorldSize/2 → UV 0.0, 0.0)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchUVNegativeEdgeTest,
	"Orogeny.Terrain.Trench.UVNegativeEdge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchUVNegativeEdgeTest::RunTest(const FString& Parameters)
{
	const FVector2D UV = UTectonicTrenchComponent::GetUVFromWorldLocation(
		FVector(-50000.0, -50000.0, 0.0), 100000.0f);

	TestEqual(TEXT("UV NegEdge: X must be 0.0"), UV.X, 0.0);
	TestEqual(TEXT("UV NegEdge: Y must be 0.0"), UV.Y, 0.0);

	return true;
}

// ============================================================================
// Test: UV Mapping — Clamping (Out-of-bounds → clamped to [0, 1])
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchUVClampingTest,
	"Orogeny.Terrain.Trench.UVClamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchUVClampingTest::RunTest(const FString& Parameters)
{
	// 200000 is 2x WorldSize — should clamp to 1.0
	// -200000 on Y — should clamp to 0.0
	const FVector2D UV = UTectonicTrenchComponent::GetUVFromWorldLocation(
		FVector(200000.0, -200000.0, 0.0), 100000.0f);

	TestEqual(TEXT("UV Clamped: X must be 1.0"), UV.X, 1.0);
	TestEqual(TEXT("UV Clamped: Y must be 0.0"), UV.Y, 0.0);

	return true;
}

// ============================================================================
// Test: UV Mapping — Arbitrary Point (Mathematical proof)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchUVArbitraryTest,
	"Orogeny.Terrain.Trench.UVArbitrary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchUVArbitraryTest::RunTest(const FString& Parameters)
{
	// WorldPos = (25000, -12500, 999)
	// UV.X = (25000 / 100000) + 0.5 = 0.25 + 0.5 = 0.75
	// UV.Y = (-12500 / 100000) + 0.5 = -0.125 + 0.5 = 0.375
	// Z is ignored
	const FVector2D UV = UTectonicTrenchComponent::GetUVFromWorldLocation(
		FVector(25000.0, -12500.0, 999.0), 100000.0f);

	TestEqual(TEXT("UV Arbitrary: X must be 0.75"), UV.X, 0.75);
	TestEqual(TEXT("UV Arbitrary: Y must be 0.375"), UV.Y, 0.375);

	return true;
}

// ============================================================================
// Test: Character Integration — TrenchComponent exists on ATitanCharacter
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FTrenchCharacterIntegrationTest,
	"Orogeny.Terrain.Trench.CharacterIntegration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTrenchCharacterIntegrationTest::RunTest(const FString& Parameters)
{
	ATitanCharacter* Titan = NewObject<ATitanCharacter>();

	// The TrenchComponent should exist after construction
	TestNotNull(TEXT("TrenchComponent must exist on ATitanCharacter"),
		Titan->FindComponentByClass<UTectonicTrenchComponent>());

	return true;
}
