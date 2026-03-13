// Copyright Orogeny. All Rights Reserved.

#include "TectonicTrenchComponent.h"
#include "Orogeny.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetRenderingLibrary.h"

// ============================================================================
// Constructor
// ============================================================================

UTectonicTrenchComponent::UTectonicTrenchComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// Tick after the movement component has updated position
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	UE_LOG(LogOrogeny, Log, TEXT("UTectonicTrenchComponent constructed: WorldSize=%.0f, RTRes=%d"),
		WorldSize, RenderTargetResolution);
}

// ============================================================================
// BeginPlay — Initialize Render Target
// ============================================================================

void UTectonicTrenchComponent::BeginPlay()
{
	Super::BeginPlay();

	// -----------------------------------------------------------------------
	// Create the persistent trench mask render target
	// -----------------------------------------------------------------------
	// RTF_R16f: Single 16-bit float channel. We only need depth/height data.
	// This is 4x smaller than RGBA8 and 8x smaller than RGBA16f, critical
	// for the 780M's shared VRAM budget.
	// -----------------------------------------------------------------------
	TrenchMaskRT = NewObject<UTextureRenderTarget2D>(this, TEXT("TrenchMaskRT"));
	TrenchMaskRT->RenderTargetFormat = ETextureRenderTargetFormat::RTF_R16f;
	TrenchMaskRT->InitAutoFormat(RenderTargetResolution, RenderTargetResolution);
	TrenchMaskRT->ClearColor = FLinearColor::White;
	TrenchMaskRT->UpdateResourceImmediate(true);

	// Clear to white (1.0) — undisturbed terrain has full height
	UKismetRenderingLibrary::ClearRenderTarget2D(
		GetWorld(), TrenchMaskRT, FLinearColor::White);

	UE_LOG(LogOrogeny, Log,
		TEXT("TrenchMaskRT initialized: %dx%d, R16f, ClearColor=White"),
		RenderTargetResolution, RenderTargetResolution);
}

// ============================================================================
// World-to-UV Mapping — The Critical Math
// ============================================================================
// This function MUST be bulletproof. If the UV is off by even 1%,
// the trench will render hundreds of units away from the Titan's feet.
//
// MAPPING:
//   UV.X = (WorldPos.X / WorldSize) + 0.5
//   UV.Y = (WorldPos.Y / WorldSize) + 0.5
//
// PROOF:
//   WorldPos = (0, 0, z)       → UV = (0.5, 0.5)         ✓ Center
//   WorldPos = (W/2, W/2, z)   → UV = (1.0, 1.0)         ✓ Max edge
//   WorldPos = (-W/2, -W/2, z) → UV = (0.0, 0.0)         ✓ Min edge
//   WorldPos = (W, -W, z)      → UV = clamped(1.5, -0.5)
//                                    = (1.0, 0.0)         ✓ Clamped
// ============================================================================

FVector2D UTectonicTrenchComponent::GetUVFromWorldLocation(
	FVector WorldLocation, float InWorldSize)
{
	// Guard against division by zero — degenerate WorldSize returns center
	if (InWorldSize <= 0.0f)
	{
		return FVector2D(0.5, 0.5);
	}

	const double U = (WorldLocation.X / static_cast<double>(InWorldSize)) + 0.5;
	const double V = (WorldLocation.Y / static_cast<double>(InWorldSize)) + 0.5;

	return FVector2D(
		FMath::Clamp(U, 0.0, 1.0),
		FMath::Clamp(V, 0.0, 1.0)
	);
}

// ============================================================================
// TickComponent — Update UV & Paint Flag
// ============================================================================

void UTectonicTrenchComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// -----------------------------------------------------------------------
	// Early-out: no owner, no RT, or no movement component
	// -----------------------------------------------------------------------
	AActor* Owner = GetOwner();
	if (!Owner || !TrenchMaskRT)
	{
		bShouldPaintThisFrame = false;
		return;
	}

	// -----------------------------------------------------------------------
	// Early-out: only paint when the Titan is actually moving
	// Saves GPU cycles when standing still (no canvas draw dispatch)
	// -----------------------------------------------------------------------
	ACharacter* Character = Cast<ACharacter>(Owner);
	if (Character && Character->GetCharacterMovement())
	{
		const float Speed = Character->GetCharacterMovement()->Velocity.Size();
		if (Speed < 1.0f)
		{
			bShouldPaintThisFrame = false;
			return;
		}
	}

	// -----------------------------------------------------------------------
	// Update current UV from world position
	// -----------------------------------------------------------------------
	CurrentTrenchUV = GetUVFromWorldLocation(Owner->GetActorLocation(), WorldSize);
	bShouldPaintThisFrame = true;
}
