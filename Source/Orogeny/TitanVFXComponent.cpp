// Copyright Orogeny. All Rights Reserved.

#include "TitanVFXComponent.h"
#include "Orogeny.h"
#include "NiagaraMath.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// ============================================================================
// Constructor
// ============================================================================

UTitanVFXComponent::UTitanVFXComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// VFX parameter updates at 20Hz — smooth visual transitions
	PrimaryComponentTick.TickInterval = 0.05f;
}

// ============================================================================
// BeginPlay — Spawn looping Niagara systems
// ============================================================================

void UTitanVFXComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Spawn looping footstep dust
	if (FootstepDustSystem)
	{
		ActiveDustComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			FootstepDustSystem,
			Owner->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true,           // bAutoActivate
			true,           // bAutoDestroy = false (looping)
			ENCPoolMethod::None,
			true            // bPreCullCheck
		);

		if (ActiveDustComponent)
		{
			// Start with zero spawn rate — tick will drive it
			ActiveDustComponent->SetFloatParameter(FName("SpawnRate"), 0.0f);
			UE_LOG(LogOrogeny, Log, TEXT("TitanVFX: Spawned FootstepDust system"));
		}
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("TitanVFX: No FootstepDustSystem assigned!"));
	}

	// Spawn looping blight smoke
	if (BlightSmokeSystem)
	{
		ActiveSmokeComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			BlightSmokeSystem,
			Owner->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true,
			true,
			ENCPoolMethod::None,
			true
		);

		if (ActiveSmokeComponent)
		{
			ActiveSmokeComponent->SetFloatParameter(FName("SmokeOpacity"), 0.0f);
			UE_LOG(LogOrogeny, Log, TEXT("TitanVFX: Spawned BlightSmoke system"));
		}
	}

	UE_LOG(LogOrogeny, Log,
		TEXT("TitanVFX: Initialized on %s"), *Owner->GetName());
}

// ============================================================================
// TickComponent — Drive VFX parameters from game state
// ============================================================================

void UTitanVFXComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// -----------------------------------------------------------------------
	// Footstep Dust — velocity drives spawn rate
	// -----------------------------------------------------------------------
	float VelocityAlpha = 0.0f;
	ACharacter* Character = Cast<ACharacter>(Owner);
	if (Character)
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			const float MaxSpeed = Movement->MaxWalkSpeed;
			if (MaxSpeed > 0.0f)
			{
				VelocityAlpha = FMath::Clamp(
					Character->GetVelocity().Size() / MaxSpeed, 0.0f, 1.0f);
			}
		}
	}

	CurrentDustRate = FNiagaraMath::CalculateDustSpawnRate(VelocityAlpha);

	if (ActiveDustComponent)
	{
		ActiveDustComponent->SetFloatParameter(FName("SpawnRate"), CurrentDustRate);
	}

	// -----------------------------------------------------------------------
	// Blight Smoke — future: read global blight ratio
	// For now, smoke drives from debug or director
	// -----------------------------------------------------------------------
	// CurrentSmokeOpacity already calculated externally or set to 0
	if (ActiveSmokeComponent)
	{
		ActiveSmokeComponent->SetFloatParameter(
			FName("SmokeOpacity"), CurrentSmokeOpacity);
	}
}

// ============================================================================
// TriggerSubductionBurst — One-shot debris explosion
// ============================================================================

void UTitanVFXComponent::TriggerSubductionBurst(float CurrentDepth)
{
	if (!SubductionDebrisSystem)
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("TitanVFX: No SubductionDebrisSystem assigned!"));
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner) return;

	const int32 BurstCount = FNiagaraMath::CalculateDebrisBurstCount(CurrentDepth);

	if (BurstCount <= 0)
	{
		return;
	}

	UNiagaraComponent* BurstComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		SubductionDebrisSystem,
		Owner->GetActorLocation(),
		Owner->GetActorRotation(),
		FVector::OneVector,
		true,   // bAutoDestroy
		true,   // bAutoActivate
		ENCPoolMethod::AutoRelease,
		true    // bPreCullCheck
	);

	if (BurstComponent)
	{
		BurstComponent->SetIntParameter(FName("BurstCount"), BurstCount);
		UE_LOG(LogOrogeny, Log,
			TEXT("TitanVFX: Debris burst at depth %.0f, count=%d"),
			CurrentDepth, BurstCount);
	}
}
