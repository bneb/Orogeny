// Copyright Orogeny. All Rights Reserved.

#include "TectonicAtmosphereComponent.h"
#include "Orogeny.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"

// ============================================================================
// Constructor
// ============================================================================

UTectonicAtmosphereComponent::UTectonicAtmosphereComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	// Niagara component is created in BeginPlay (needs a world context)

	UE_LOG(LogOrogeny, Log,
		TEXT("UTectonicAtmosphereComponent constructed: BaseRadius=%.0f, MaxMult=%.1f"),
		BaseDisplacementRadius, MaxVelocityMultiplier);
}

// ============================================================================
// BeginPlay — Spawn Niagara System
// ============================================================================

void UTectonicAtmosphereComponent::BeginPlay()
{
	Super::BeginPlay();

	// -----------------------------------------------------------------------
	// Spawn the Niagara dust wake system
	// -----------------------------------------------------------------------
	if (DustWakeAsset)
	{
		DustWakeSystem = NewObject<UNiagaraComponent>(GetOwner(), TEXT("DustWakeSystem"));
		if (DustWakeSystem)
		{
			DustWakeSystem->SetAsset(DustWakeAsset);
			DustWakeSystem->SetupAttachment(this);
			DustWakeSystem->RegisterComponent();
			DustWakeSystem->Activate(true);

			UE_LOG(LogOrogeny, Log, TEXT("DustWakeSystem spawned and attached."));
		}
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("DustWakeAsset not assigned — no wake particles will spawn. "
			     "Assign in the Blueprint/Editor."));
	}
}

// ============================================================================
// Velocity-to-Radius — The Critical Math
// ============================================================================
// This function MUST be deterministic and pure.
//
// FORMULA:
//   SpeedAlpha = Clamp(CurrentSpeed / MaxSpeed, 0, 1)
//   Radius = BaseRadius * (1.0 + SpeedAlpha * (Multiplier - 1.0))
//
// PROOF TABLE:
//   Speed=0,   MaxSpeed=150, Base=5000, Mult=2 → Alpha=0.0 → 5000 * 1.0 = 5000
//   Speed=75,  MaxSpeed=150, Base=5000, Mult=2 → Alpha=0.5 → 5000 * 1.5 = 7500
//   Speed=150, MaxSpeed=150, Base=5000, Mult=2 → Alpha=1.0 → 5000 * 2.0 = 10000
//   Speed=300, MaxSpeed=150, Base=5000, Mult=2 → Alpha=1.0 → 5000 * 2.0 = 10000 (clamped)
// ============================================================================

float UTectonicAtmosphereComponent::CalculateDisplacementRadius(
	float CurrentSpeed, float MaxSpeed,
	float BaseRadius, float Multiplier)
{
	// Guard against division by zero — if MaxSpeed is 0, return base
	if (MaxSpeed <= 0.0f)
	{
		return BaseRadius;
	}

	const float SpeedAlpha = FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f);
	return BaseRadius * (1.0f + SpeedAlpha * (Multiplier - 1.0f));
}

// ============================================================================
// TickComponent — Push Data to MPC + Niagara
// ============================================================================

void UTectonicAtmosphereComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// -----------------------------------------------------------------------
	// Read velocity from the owning character
	// -----------------------------------------------------------------------
	float CurrentSpeed = 0.0f;
	float MaxSpeed = 150.0f; // Fallback default

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (Character && Character->GetCharacterMovement())
	{
		CurrentSpeed = Character->GetCharacterMovement()->Velocity.Size();
		MaxSpeed = Character->GetCharacterMovement()->GetMaxSpeed();
	}

	// -----------------------------------------------------------------------
	// Calculate displacement radius
	// -----------------------------------------------------------------------
	CurrentDisplacementRadius = CalculateDisplacementRadius(
		CurrentSpeed, MaxSpeed, BaseDisplacementRadius, MaxVelocityMultiplier);

	CurrentWakeIntensity = (MaxSpeed > 0.0f)
		? FMath::Clamp(CurrentSpeed / MaxSpeed, 0.0f, 1.0f)
		: 0.0f;

	// -----------------------------------------------------------------------
	// Push to Material Parameter Collection (null-safe)
	// -----------------------------------------------------------------------
	if (AtmosphereMPC && GetWorld())
	{
		const FVector Location = Owner->GetActorLocation();

		UKismetMaterialLibrary::SetVectorParameterValue(
			GetWorld(), AtmosphereMPC, MPCPositionParameterName,
			FLinearColor(Location.X, Location.Y, Location.Z, 1.0f));

		UKismetMaterialLibrary::SetScalarParameterValue(
			GetWorld(), AtmosphereMPC, MPCRadiusParameterName,
			CurrentDisplacementRadius);
	}

	// -----------------------------------------------------------------------
	// Push to Niagara (null-safe)
	// -----------------------------------------------------------------------
	if (DustWakeSystem)
	{
		DustWakeSystem->SetVariableFloat(FName("WakeIntensity"), CurrentWakeIntensity);
	}
}
