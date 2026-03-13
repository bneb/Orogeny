// Copyright Orogeny. All Rights Reserved.
//
// ============================================================================
// OROGENY TDD: Procedural Scale Camera & Haptics Verification
// ============================================================================
//
// RED-TO-GREEN WORKFLOW:
// These tests verify camera configuration, shake pattern setup, and
// footstep notify null safety without needing a live UWorld or PIE.
//
// DESIGN TARGETS:
//   - TargetArmLength: 3500 cm (massive pull-back)
//   - SocketOffset.Z: 800 cm (looking down from clouds)
//   - CameraLagSpeed: 1.5 (very slow drone follow)
//   - Shake Duration: 0.5s per footfall
//   - Shake Z Amplitude: 15.0 (vertical heave)
//   - Shake Frequency: 10.0 Hz (deep rumble)
//
// RUN: UnrealEditor-Cmd ... -ExecCmds="Automation RunTests Orogeny.Camera"
// ============================================================================

#include "Misc/AutomationTest.h"
#include "TitanCharacter.h"
#include "TectonicFootstepShake.h"
#include "AnimNotify_TectonicStep.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"

// ============================================================================
// Test 1: Camera Scale Configuration
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCameraScaleConfigTest,
	"Orogeny.Camera.ScaleConfiguration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCameraScaleConfigTest::RunTest(const FString& Parameters)
{
	ATitanCharacter* Titan = NewObject<ATitanCharacter>();

	USpringArmComponent* Boom = Titan->GetCameraBoom();
	TestNotNull(TEXT("CameraBoom must exist"), Boom);

	if (!Boom) return false;

	// Arm length — extreme pull-back for colossal scale
	TestEqual(TEXT("TargetArmLength must be 3500"),
		Boom->TargetArmLength, 3500.0f);

	// Socket offset — elevated viewpoint, looking down
	TestEqual(TEXT("SocketOffset.Z must be 800"),
		Boom->SocketOffset.Z, 800.0);

	TestEqual(TEXT("SocketOffset.X must be 0"),
		Boom->SocketOffset.X, 0.0);

	TestEqual(TEXT("SocketOffset.Y must be 0"),
		Boom->SocketOffset.Y, 0.0);

	// Camera lag — slow, heavy drone feel
	TestTrue(TEXT("bEnableCameraLag must be true"),
		Boom->bEnableCameraLag);

	TestEqual(TEXT("CameraLagSpeed must be 1.5"),
		Boom->CameraLagSpeed, 1.5f);

	// Rotation lag
	TestTrue(TEXT("bEnableCameraRotationLag must be true"),
		Boom->bEnableCameraRotationLag);

	TestEqual(TEXT("CameraRotationLagSpeed must be 2.0"),
		Boom->CameraRotationLagSpeed, 2.0f);

	// Pawn control rotation
	TestTrue(TEXT("bUsePawnControlRotation must be true"),
		Boom->bUsePawnControlRotation);

	return true;
}

// ============================================================================
// Test 2: Camera Shake Pattern Verification
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCameraShakePatternTest,
	"Orogeny.Camera.ShakePattern",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCameraShakePatternTest::RunTest(const FString& Parameters)
{
	UTectonicFootstepShake* Shake = NewObject<UTectonicFootstepShake>();
	TestNotNull(TEXT("UTectonicFootstepShake must instantiate"), Shake);

	if (!Shake) return false;

	// Verify the root pattern is Perlin noise
	UCameraShakePattern* Pattern = Shake->GetRootShakePattern();
	TestNotNull(TEXT("Root shake pattern must exist"), Pattern);

	if (!Pattern) return false;

	UPerlinNoiseCameraShakePattern* PerlinPattern = Cast<UPerlinNoiseCameraShakePattern>(Pattern);
	TestNotNull(TEXT("Root pattern must be UPerlinNoiseCameraShakePattern"), PerlinPattern);

	if (!PerlinPattern) return false;

	// Verify duration
	TestTrue(TEXT("Shake duration must be > 0"),
		PerlinPattern->Duration > 0.0f);

	TestEqual(TEXT("Shake duration must be 0.5s"),
		PerlinPattern->Duration, UTectonicFootstepShake::SHAKE_DURATION);

	// Verify Z amplitude (primary earthquake axis)
	TestEqual(TEXT("Z amplitude must be 15"),
		PerlinPattern->Z.Amplitude, UTectonicFootstepShake::LOCATION_AMPLITUDE_Z);

	// Verify Pitch amplitude
	TestEqual(TEXT("Pitch amplitude must be 2"),
		PerlinPattern->Pitch.Amplitude, UTectonicFootstepShake::ROTATION_AMPLITUDE_PITCH);

	// Verify frequency
	TestEqual(TEXT("Z frequency must be 10 Hz"),
		PerlinPattern->Z.Frequency, UTectonicFootstepShake::NOISE_FREQUENCY);

	// Verify non-primary axes are silent
	TestEqual(TEXT("X amplitude must be 0 (no forward/back)"),
		PerlinPattern->X.Amplitude, 0.0f);

	TestEqual(TEXT("Y amplitude must be 0 (no lateral)"),
		PerlinPattern->Y.Amplitude, 0.0f);

	TestEqual(TEXT("Yaw amplitude must be 0 (no horizontal wobble)"),
		PerlinPattern->Yaw.Amplitude, 0.0f);

	TestEqual(TEXT("Roll amplitude must be 0 (no barrel roll)"),
		PerlinPattern->Roll.Amplitude, 0.0f);

	return true;
}

// ============================================================================
// Test 3: Camera Shake Constants Consistency
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCameraShakeConstantsTest,
	"Orogeny.Camera.ShakeConstants",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCameraShakeConstantsTest::RunTest(const FString& Parameters)
{
	// Verify the constexpr design constants
	TestEqual(TEXT("SHAKE_DURATION must be 0.5"),
		UTectonicFootstepShake::SHAKE_DURATION, 0.5f);

	TestEqual(TEXT("LOCATION_AMPLITUDE_Z must be 15"),
		UTectonicFootstepShake::LOCATION_AMPLITUDE_Z, 15.0f);

	TestEqual(TEXT("ROTATION_AMPLITUDE_PITCH must be 2"),
		UTectonicFootstepShake::ROTATION_AMPLITUDE_PITCH, 2.0f);

	TestEqual(TEXT("NOISE_FREQUENCY must be 10"),
		UTectonicFootstepShake::NOISE_FREQUENCY, 10.0f);

	// Verify design constraints:
	// Duration < 1s (one footfall, not continuous)
	TestTrue(TEXT("Duration must be < 1s (single footfall, not continuous)"),
		UTectonicFootstepShake::SHAKE_DURATION < 1.0f);

	// Z amplitude > Pitch amplitude (vertical heave dominates)
	TestTrue(TEXT("Z amplitude must dominate over Pitch rotation"),
		UTectonicFootstepShake::LOCATION_AMPLITUDE_Z > UTectonicFootstepShake::ROTATION_AMPLITUDE_PITCH);

	return true;
}

// ============================================================================
// Test 4: Footstep Notify Null Safety
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFootstepNotifyNullSafetyTest,
	"Orogeny.Camera.NotifyNullSafety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFootstepNotifyNullSafetyTest::RunTest(const FString& Parameters)
{
	UAnimNotify_TectonicStep* Notify = NewObject<UAnimNotify_TectonicStep>();
	TestNotNull(TEXT("UAnimNotify_TectonicStep must instantiate"), Notify);

	// Calling Notify with null MeshComp must not crash
	FAnimNotifyEventReference DummyRef;
	Notify->Notify(nullptr, nullptr, DummyRef);

	// If we reach here, null safety is confirmed
	TestTrue(TEXT("Notify with null MeshComp must not crash"), true);

	return true;
}

// ============================================================================
// Test 5: Footstep Notify Default Properties
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FFootstepNotifyDefaultsTest,
	"Orogeny.Camera.NotifyDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FFootstepNotifyDefaultsTest::RunTest(const FString& Parameters)
{
	UAnimNotify_TectonicStep* Notify = NewObject<UAnimNotify_TectonicStep>();

	// Default shake scale should be 1.0 (full intensity)
	TestEqual(TEXT("Default ShakeScale must be 1.0"),
		Notify->ShakeScale, 1.0f);

	// Default force feedback should be null (set by tech art in editor)
	TestNull(TEXT("Default ForceFeedbackEffect must be null"),
		Notify->ForceFeedbackEffect.Get());

	// Display name
	TestEqual(TEXT("Notify name must be 'Tectonic Footstep'"),
		Notify->GetNotifyName(), FString(TEXT("Tectonic Footstep")));

	return true;
}

// ============================================================================
// Test 6: Camera FOV and Follow Camera Verification
// ============================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCameraFollowTest,
	"Orogeny.Camera.FollowCamera",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FCameraFollowTest::RunTest(const FString& Parameters)
{
	ATitanCharacter* Titan = NewObject<ATitanCharacter>();

	UCameraComponent* Camera = Titan->GetFollowCamera();
	TestNotNull(TEXT("FollowCamera must exist"), Camera);

	if (!Camera) return false;

	// FOV — wide for landscape vistas
	TestEqual(TEXT("FollowCamera FOV must be 75"),
		Camera->FieldOfView, 75.0f);

	// Camera should NOT use pawn control rotation (boom handles it)
	TestFalse(TEXT("FollowCamera must NOT use pawn control rotation"),
		Camera->bUsePawnControlRotation);

	return true;
}
