// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Foot Placement IK Math Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// These tests verify the PURE MATH IK solver without any UWorld dependency.
// We construct FFootIKTraceResult structs with mock data and assert that
// CalculateFootIKFromTraceResults produces mathematically correct output.
//
// TEST SCENARIOS:
// 1. Flat ground — both feet at same Z, normal up → zero offsets
// 2. Left foot high, right foot low → hip drops, left pushes up
// 3. Steep forward incline → foot pitch rotates to match slope
// 4. One foot no ground contact → graceful degradation
// 5. Symmetric slope — both feet equally offset → equal IK
// 6. Foot rotation on a side slope → roll rotation
//
// RUN: Session Frontend → Automation → Filter "Orogeny.Animation"
// CLI: UnrealEditor-Cmd.exe ... -ExecCmds="Automation RunTests Orogeny.Animation"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "TitanAnimInstance.h"

// ============================================================================
// Helper: Build a mock FFootIKTraceResult
// ============================================================================
namespace OrogenyTestHelpers
{
	/**
	 * Constructs a mock foot trace result for testing.
	 *
	 * @param FootZ       Z position of the foot bone
	 * @param GroundZ     Z position of the ground hit point (ignored if bHit=false)
	 * @param Normal      Surface normal at the hit point
	 * @param bHit        Whether the trace hit anything
	 * @return Populated FFootIKTraceResult
	 */
	static FFootIKTraceResult MakeTrace(
		float FootZ,
		float GroundZ,
		FVector Normal = FVector::UpVector,
		bool bHit = true)
	{
		FFootIKTraceResult Result;
		Result.bHitGround = bHit;
		Result.FootBoneLocation = FVector(0.0f, 0.0f, FootZ);
		Result.ImpactLocation = FVector(0.0f, 0.0f, GroundZ);
		Result.ImpactNormal = Normal;
		Result.FootBaseOffset = 0.0f;
		return Result;
	}
}

// ============================================================================
// Test 1: Flat Ground — Zero Offsets
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FIKMathFlatGroundTest,
	"Orogeny.Animation.FootPlacementIK.FlatGround",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FIKMathFlatGroundTest::RunTest(const FString& Parameters)
{
	using namespace OrogenyTestHelpers;

	// Both feet at Z=0, ground at Z=0, normal straight up
	FFootIKTraceResult LeftTrace = MakeTrace(0.0f, 0.0f);
	FFootIKTraceResult RightTrace = MakeTrace(0.0f, 0.0f);

	FFootIKResult LeftResult, RightResult;
	float HipOffset = 0.0f;

	UTitanAnimInstance::CalculateFootIKFromTraceResults(
		LeftTrace, RightTrace, LeftResult, RightResult, HipOffset);

	// On flat ground: no hip drop, no IK offsets, no rotation
	TestEqual(TEXT("Flat ground: HipOffset must be 0"),
		HipOffset, 0.0f);

	TestEqual(TEXT("Flat ground: Left IK Z offset must be 0"),
		LeftResult.IKOffset.Z, 0.0);

	TestEqual(TEXT("Flat ground: Right IK Z offset must be 0"),
		RightResult.IKOffset.Z, 0.0);

	TestEqual(TEXT("Flat ground: Left foot pitch must be 0"),
		LeftResult.FootRotation.Pitch, 0.0);

	TestEqual(TEXT("Flat ground: Left foot roll must be 0"),
		LeftResult.FootRotation.Roll, 0.0);

	TestEqual(TEXT("Flat ground: Right foot pitch must be 0"),
		RightResult.FootRotation.Pitch, 0.0);

	TestEqual(TEXT("Flat ground: Right foot roll must be 0"),
		RightResult.FootRotation.Roll, 0.0);

	return true;
}

// ============================================================================
// Test 2: Left Foot High, Right Foot Low (Rock / Depression)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FIKMathUnevenGroundTest,
	"Orogeny.Animation.FootPlacementIK.UnevenGround",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FIKMathUnevenGroundTest::RunTest(const FString& Parameters)
{
	using namespace OrogenyTestHelpers;

	// Left foot bone at Z=0, ground at Z=+30 (on a rock — ground is ABOVE foot)
	// Right foot bone at Z=0, ground at Z=-20 (in a dip — ground is BELOW foot)
	FFootIKTraceResult LeftTrace = MakeTrace(0.0f, 30.0f);
	FFootIKTraceResult RightTrace = MakeTrace(0.0f, -20.0f);

	FFootIKResult LeftResult, RightResult;
	float HipOffset = 0.0f;

	UTitanAnimInstance::CalculateFootIKFromTraceResults(
		LeftTrace, RightTrace, LeftResult, RightResult, HipOffset);

	// Hip must drop by the MINIMUM of the two deltas
	// min(+30, -20) = -20 → hips drop 20 units
	TestEqual(TEXT("Uneven ground: HipOffset must be -20"),
		HipOffset, -20.0f);

	// Left foot IK: delta(+30) - hipOffset(-20) = +50 (push up 50 units)
	TestEqual(TEXT("Uneven ground: Left IK Z must be +50"),
		LeftResult.IKOffset.Z, 50.0);

	// Right foot IK: delta(-20) - hipOffset(-20) = 0 (at lowered hip level)
	TestEqual(TEXT("Uneven ground: Right IK Z must be 0"),
		RightResult.IKOffset.Z, 0.0);

	return true;
}

// ============================================================================
// Test 3: Steep Forward Incline (30° Slope)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FIKMathSteepInclineTest,
	"Orogeny.Animation.FootPlacementIK.SteepIncline",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FIKMathSteepInclineTest::RunTest(const FString& Parameters)
{
	using namespace OrogenyTestHelpers;

	// 30° forward slope: normal = (sin30, 0, cos30) = (0.5, 0, 0.866)
	const FVector SlopeNormal(0.5f, 0.0f, 0.866f);

	// Both feet at same Z, ground at same Z (the slope manifests via normal)
	FFootIKTraceResult LeftTrace = MakeTrace(0.0f, 0.0f, SlopeNormal);
	FFootIKTraceResult RightTrace = MakeTrace(0.0f, 0.0f, SlopeNormal);

	FFootIKResult LeftResult, RightResult;
	float HipOffset = 0.0f;

	UTitanAnimInstance::CalculateFootIKFromTraceResults(
		LeftTrace, RightTrace, LeftResult, RightResult, HipOffset);

	// On equal footing (same Z delta): hip offset should be 0
	TestEqual(TEXT("Incline: HipOffset must be 0 (symmetric)"),
		HipOffset, 0.0f);

	// Foot should pitch forward to match the 30° slope
	// atan2(0.5, 0.866) ≈ 30°
	const float ExpectedPitch = FMath::RadiansToDegrees(FMath::Atan2(0.5f, 0.866f));

	TestTrue(TEXT("Incline: Left foot pitch must be ~30 degrees"),
		FMath::IsNearlyEqual(LeftResult.FootRotation.Pitch, ExpectedPitch, 1.0f));

	TestTrue(TEXT("Incline: Right foot pitch must be ~30 degrees"),
		FMath::IsNearlyEqual(RightResult.FootRotation.Pitch, ExpectedPitch, 1.0f));

	// No roll on a pure forward slope
	TestTrue(TEXT("Incline: Left foot roll must be ~0"),
		FMath::IsNearlyEqual(LeftResult.FootRotation.Roll, 0.0f, 0.1f));

	return true;
}

// ============================================================================
// Test 4: One Foot Has No Ground Contact (Cliff Edge / Flying)
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FIKMathNoGroundContactTest,
	"Orogeny.Animation.FootPlacementIK.NoGroundContact",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FIKMathNoGroundContactTest::RunTest(const FString& Parameters)
{
	using namespace OrogenyTestHelpers;

	// Left foot on solid ground
	FFootIKTraceResult LeftTrace = MakeTrace(0.0f, 10.0f);

	// Right foot finds NO ground (cliff edge, trace miss)
	FFootIKTraceResult RightTrace = MakeTrace(0.0f, 0.0f, FVector::UpVector, false);

	FFootIKResult LeftResult, RightResult;
	float HipOffset = 0.0f;

	UTitanAnimInstance::CalculateFootIKFromTraceResults(
		LeftTrace, RightTrace, LeftResult, RightResult, HipOffset);

	// Right foot has no ground → its delta is 0
	// min(+10, 0) = 0 → hip doesn't need to drop
	TestEqual(TEXT("No contact: HipOffset must be 0"),
		HipOffset, 0.0f);

	// Right foot with no ground contact should have zero IK offset
	TestEqual(TEXT("No contact: Right IK must be zero vector"),
		RightResult.IKOffset, FVector::ZeroVector);

	// Right foot rotation should be zero (no surface to align to)
	TestEqual(TEXT("No contact: Right rotation pitch must be 0"),
		RightResult.FootRotation.Pitch, 0.0);
	TestEqual(TEXT("No contact: Right rotation roll must be 0"),
		RightResult.FootRotation.Roll, 0.0);

	// Left foot should still have valid IK
	TestEqual(TEXT("No contact: Left IK Z must be +10"),
		LeftResult.IKOffset.Z, 10.0);

	return true;
}

// ============================================================================
// Test 5: Side Slope — Roll Rotation
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FIKMathSideSlopeTest,
	"Orogeny.Animation.FootPlacementIK.SideSlope",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FIKMathSideSlopeTest::RunTest(const FString& Parameters)
{
	// 20° side slope: normal tilted in Y direction
	// Normal = (0, sin20, cos20) ≈ (0, 0.342, 0.940)
	const FVector SideNormal(0.0f, 0.342f, 0.940f);

	// Test the rotation function directly
	FRotator Rotation = UTitanAnimInstance::CalculateFootRotationFromNormal(SideNormal);

	// Should produce roll but no pitch
	const float ExpectedRoll = FMath::RadiansToDegrees(FMath::Atan2(0.342f, 0.940f));

	TestTrue(TEXT("Side slope: Pitch must be ~0"),
		FMath::IsNearlyEqual(Rotation.Pitch, 0.0f, 0.5f));

	TestTrue(TEXT("Side slope: Roll must be ~20 degrees"),
		FMath::IsNearlyEqual(Rotation.Roll, ExpectedRoll, 1.0f));

	return true;
}

// ============================================================================
// Test 6: Extreme Depression — Hip Offset Clamping
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FIKMathHipClampTest,
	"Orogeny.Animation.FootPlacementIK.HipOffsetClamping",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FIKMathHipClampTest::RunTest(const FString& Parameters)
{
	using namespace OrogenyTestHelpers;

	// Extreme scenario: one foot bone at Z=0, ground way below at Z=-200
	// Without clamping, the hips would drop 200 units (absurd)
	FFootIKTraceResult LeftTrace = MakeTrace(0.0f, 0.0f);
	FFootIKTraceResult RightTrace = MakeTrace(0.0f, -200.0f);

	FFootIKResult LeftResult, RightResult;
	float HipOffset = 0.0f;

	UTitanAnimInstance::CalculateFootIKFromTraceResults(
		LeftTrace, RightTrace, LeftResult, RightResult, HipOffset);

	// Hip offset must be clamped to -50 (the MaxIKOffset default)
	TestEqual(TEXT("Extreme depression: HipOffset must be clamped to -50"),
		HipOffset, -50.0f);

	// Hip offset should never be positive
	TestTrue(TEXT("HipOffset must be <= 0"),
		HipOffset <= 0.0f);

	return true;
}
