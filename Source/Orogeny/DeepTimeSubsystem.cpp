// Copyright Orogeny. All Rights Reserved.

#include "DeepTimeSubsystem.h"
#include "Orogeny.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

// ============================================================================
// Pure Math — CalculateTimeScale
// ============================================================================
// Maps velocity alpha [0, 1] → time scale [1, 36525].
// Linear interpolation. Clamped input.
//
// PROOF:
//   Alpha 0.0 → Lerp(1, 36525, 0) = 1.0    (1 day/min)
//   Alpha 1.0 → Lerp(1, 36525, 1) = 36525.0 (1 century/min)
//   Alpha 2.0 → Clamped to 1.0 → 36525.0
//   Alpha -1  → Clamped to 0.0 → 1.0
// ============================================================================

float UDeepTimeSubsystem::CalculateTimeScale(float VelocityAlpha)
{
	const float ClampedAlpha = FMath::Clamp(VelocityAlpha, 0.0f, 1.0f);
	return FMath::Lerp(1.0f, MAX_TIME_SCALE, ClampedAlpha);
}

// ============================================================================
// Pure Math — AdvanceDay
// ============================================================================
// DaysPerSecond = TimeScale / BaseDayDuration
// DayDelta = DaysPerSecond * DeltaTime
//
// PROOF (base rate):
//   TimeScale=1, Δt=1s, Base=60 → 1/60 = 0.01667 days added
//
// PROOF (max rate):
//   TimeScale=36525, Δt=1s, Base=60 → 36525/60 = 608.75 days added
//   Over 60s: 608.75 × 60 = 36525 days = 1 century ✓
// ============================================================================

double UDeepTimeSubsystem::AdvanceDay(double CurrentDayValue, float TimeScale,
	float DeltaTime, float BaseDayDuration)
{
	const double DaysPerSecond = static_cast<double>(TimeScale) / static_cast<double>(BaseDayDuration);
	return CurrentDayValue + (DaysPerSecond * static_cast<double>(DeltaTime));
}

// ============================================================================
// Pure Math — CalculateSunAngle
// ============================================================================
// Extracts fractional part of day, maps to [0, 360).
// 0.0 = midnight, 0.5 = noon (180°), wraps at whole numbers.
// ============================================================================

float UDeepTimeSubsystem::CalculateSunAngle(double FractionalDay)
{
	const double DayFraction = FractionalDay - FMath::FloorToDouble(FractionalDay);
	return static_cast<float>(DayFraction * 360.0);
}

// ============================================================================
// Pure Math — CalculateSeasonAlpha
// ============================================================================
// Period = 365.25 days. Extracts fractional year position.
// 0.0 = winter solstice, 0.5 = summer solstice.
// ============================================================================

float UDeepTimeSubsystem::CalculateSeasonAlpha(double FractionalDay)
{
	const double YearFraction = FractionalDay / DAYS_PER_YEAR;
	const double SeasonPosition = YearFraction - FMath::FloorToDouble(YearFraction);
	return static_cast<float>(SeasonPosition);
}

// ============================================================================
// Subsystem Lifecycle
// ============================================================================

void UDeepTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentDay = 0.0;
	CurrentTimeScale = 1.0f;
	CurrentVelocityAlpha = 0.0f;
	CurrentSunAngle = 180.0f; // Start at noon
	LastBroadcastYear = -1;
	LastBroadcastCentury = -1;
	SunLightActor = nullptr;

	UE_LOG(LogOrogeny, Log,
		TEXT("UDeepTimeSubsystem initialized. BaseDayDuration=%.0fs, MaxTimeScale=%.0f"),
		BASE_DAY_DURATION, MAX_TIME_SCALE);
}

void UDeepTimeSubsystem::Deinitialize()
{
	UE_LOG(LogOrogeny, Log,
		TEXT("UDeepTimeSubsystem deinitializing. Final day: %.2f (%.1f years)"),
		CurrentDay, CurrentDay / DAYS_PER_YEAR);

	Super::Deinitialize();
}

TStatId UDeepTimeSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDeepTimeSubsystem, STATGROUP_Tickables);
}

// ============================================================================
// Tick — The Deep Time Loop
// ============================================================================

void UDeepTimeSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -----------------------------------------------------------------------
	// 1. Read player velocity alpha
	// -----------------------------------------------------------------------
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (PlayerPawn)
	{
		// Use horizontal velocity only — falling should NOT compress time
		const FVector Vel = PlayerPawn->GetVelocity();
		const float HorizontalSpeed = FVector(Vel.X, Vel.Y, 0.0f).Size();
		const ACharacter* AsCharacter = Cast<ACharacter>(PlayerPawn);
		const float MaxSpeed = (AsCharacter && AsCharacter->GetCharacterMovement())
			? AsCharacter->GetCharacterMovement()->MaxWalkSpeed
			: 150.0f;
		CurrentVelocityAlpha = FMath::Clamp(HorizontalSpeed / MaxSpeed, 0.0f, 1.0f);
	}

	// -----------------------------------------------------------------------
	// 2. Calculate time scale from velocity
	// -----------------------------------------------------------------------
	CurrentTimeScale = CalculateTimeScale(CurrentVelocityAlpha);

	// -----------------------------------------------------------------------
	// 3. Advance deep time
	// -----------------------------------------------------------------------
	CurrentDay = AdvanceDay(CurrentDay, CurrentTimeScale, DeltaTime, BASE_DAY_DURATION);

	// -----------------------------------------------------------------------
	// 4. Derive sun angle
	// -----------------------------------------------------------------------
	CurrentSunAngle = CalculateSunAngle(CurrentDay);

	// -----------------------------------------------------------------------
	// 5. Update directional light rotation
	// -----------------------------------------------------------------------
	UpdateSunLight();

	// -----------------------------------------------------------------------
	// 6. Push to MPC for material reactions
	// -----------------------------------------------------------------------
	if (DeepTimeMPC)
	{
		UKismetMaterialLibrary::SetScalarParameterValue(
			World, DeepTimeMPC, MPCSunAngleParam, CurrentSunAngle);
		UKismetMaterialLibrary::SetScalarParameterValue(
			World, DeepTimeMPC, MPCSeasonAlphaParam, CalculateSeasonAlpha(CurrentDay));
		UKismetMaterialLibrary::SetScalarParameterValue(
			World, DeepTimeMPC, MPCTimeScaleParam, CurrentTimeScale);
	}

	// -----------------------------------------------------------------------
	// 7. Check milestone broadcasts
	// -----------------------------------------------------------------------
	CheckMilestones();
}

// ============================================================================
// Sun Light Update — Lazy-find tagged directional light
// ============================================================================

void UDeepTimeSubsystem::UpdateSunLight()
{
	if (!SunLightActor)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), SunLightTag, Found);
		if (Found.Num() > 0)
		{
			SunLightActor = Found[0];
			UE_LOG(LogOrogeny, Log, TEXT("DeepTime: Found sun light actor '%s'"),
				*SunLightActor->GetName());
		}
	}

	if (SunLightActor)
	{
		FRotator SunRotation = SunLightActor->GetActorRotation();
		SunRotation.Pitch = -CurrentSunAngle;
		SunLightActor->SetActorRotation(SunRotation);
	}
}

// ============================================================================
// Milestone Broadcasts — Year / Century boundaries
// ============================================================================

void UDeepTimeSubsystem::CheckMilestones()
{
	const int32 CurrentYear = static_cast<int32>(CurrentDay / DAYS_PER_YEAR);
	const int32 CurrentCenturyNum = static_cast<int32>(CurrentDay / DAYS_PER_CENTURY);

	if (CurrentYear > LastBroadcastYear)
	{
		LastBroadcastYear = CurrentYear;
		OnNewYear.Broadcast(CurrentYear);

		if (CurrentYear % 100 == 0)
		{
			UE_LOG(LogOrogeny, Log,
				TEXT("DeepTime: New Century %d (Day %.0f)"), CurrentCenturyNum, CurrentDay);
		}
	}

	if (CurrentCenturyNum > LastBroadcastCentury)
	{
		LastBroadcastCentury = CurrentCenturyNum;
		OnNewCentury.Broadcast(CurrentCenturyNum);

		UE_LOG(LogOrogeny, Warning,
			TEXT("=== DEEP TIME: Century %d reached (Day %.0f / Year %d) ==="),
			CurrentCenturyNum, CurrentDay, CurrentYear);
	}
}
