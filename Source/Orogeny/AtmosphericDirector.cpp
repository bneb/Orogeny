// Copyright Orogeny. All Rights Reserved.

#include "AtmosphericDirector.h"
#include "Orogeny.h"
#include "AtmosphericMath.h"
#include "DeepTimeSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Atmosphere/AtmosphericFogComponent.h"

// ============================================================================
// Constructor
// ============================================================================

AAtmosphericDirector::AAtmosphericDirector()
{
	PrimaryActorTick.bCanEverTick = true;

	// Atmosphere changes are smooth gradients — 10Hz is plenty
	PrimaryActorTick.TickInterval = 0.1f;
}

// ============================================================================
// BeginPlay — Find and cache sky/fog actors
// ============================================================================

void AAtmosphericDirector::BeginPlay()
{
	Super::BeginPlay();

	// Find AExponentialHeightFog
	TArray<AActor*> FogActors;
	UGameplayStatics::GetAllActorsOfClass(
		this, AExponentialHeightFog::StaticClass(), FogActors);
	if (FogActors.Num() > 0)
	{
		HeightFogActor = FogActors[0];
		UE_LOG(LogOrogeny, Log, TEXT("AtmosphericDirector: Found HeightFog: %s"),
			*HeightFogActor->GetName());
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("AtmosphericDirector: No AExponentialHeightFog in level!"));
	}

	// Find ASkyAtmosphere — use generic actor search by class name
	// since ASkyAtmosphere may not be directly available in all builds
	TArray<AActor*> SkyActors;
	UGameplayStatics::GetAllActorsOfClass(
		this, ASkyAtmosphere::StaticClass(), SkyActors);
	if (SkyActors.Num() > 0)
	{
		SkyAtmosphereActor = SkyActors[0];
		UE_LOG(LogOrogeny, Log, TEXT("AtmosphericDirector: Found SkyAtmosphere: %s"),
			*SkyAtmosphereActor->GetName());
	}
	else
	{
		UE_LOG(LogOrogeny, Warning,
			TEXT("AtmosphericDirector: No ASkyAtmosphere in level!"));
	}

	UE_LOG(LogOrogeny, Log,
		TEXT("AtmosphericDirector: Initialized. BaseFog=%.4f, DebugBlight=%.2f"),
		BaseFogDensity, DebugBlightRatio);
}

// ============================================================================
// Tick — Drive atmosphere from Deep Time + Blight
// ============================================================================

void AAtmosphericDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Read sun angle from Deep Time
	float SunAngle = 0.0f;
	if (UWorld* World = GetWorld())
	{
		if (UDeepTimeSubsystem* DeepTime = World->GetSubsystem<UDeepTimeSubsystem>())
		{
			SunAngle = DeepTime->CurrentSunAngle;
		}
	}

	// Calculate atmospheric state
	CurrentTimeOfDayAlpha = FAtmosphericMath::EvaluateTimeOfDayAlpha(SunAngle);
	CurrentBlightRatio = ResolveBlightRatio();
	CurrentFogDensity = FAtmosphericMath::CalculateFogDensity(
		BaseFogDensity, CurrentBlightRatio);

	const FLinearColor CurrentSkyColor = FAtmosphericMath::BlendScatteringColor(
		HealthySkyColor, BlightedSkyColor, CurrentBlightRatio);

	// -----------------------------------------------------------------------
	// Apply to ExponentialHeightFog
	// -----------------------------------------------------------------------
	if (HeightFogActor)
	{
		AExponentialHeightFog* FogActor = Cast<AExponentialHeightFog>(HeightFogActor.Get());
		if (FogActor && FogActor->GetComponent())
		{
			FogActor->GetComponent()->SetFogDensity(CurrentFogDensity);

			// Tint the fog inscattering color toward blight
			FogActor->GetComponent()->SetFogInscatteringColor(CurrentSkyColor);
		}
	}

	// SkyAtmosphere property application is deferred to Tech Artist
	// blueprint integration, as USkyAtmosphereComponent properties
	// vary significantly between UE versions. The math is proven in TDD.
}

// ============================================================================
// ResolveBlightRatio — Debug override or future system query
// ============================================================================

float AAtmosphericDirector::ResolveBlightRatio() const
{
	if (DebugBlightRatio >= 0.0f)
	{
		return FMath::Clamp(DebugBlightRatio, 0.0f, 1.0f);
	}

	// Future: Query GameMode or PangeaManager for global blight ratio
	// For now, return 0 (pristine nature)
	return 0.0f;
}
