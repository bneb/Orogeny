# Deep Time: Chrono-Tick Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** UDeepTimeSubsystem (Sprint 1)

## Architecture

```
UDeepTimeSubsystem (UWorldSubsystem — auto-singleton)
  ├─ Velocity Alpha → CalculateTimeScale() → AdvanceDay()
  ├─ CurrentDay (double) → CalculateSunAngle() → Directional Light
  ├─ CurrentDay → CalculateSeasonAlpha() → MPC
  ├─ MPC Bridge: SunAngle, SeasonAlpha, TimeScale
  └─ Delegates: OnNewYear, OnNewCentury
```

## Step 1: Tag the Directional Light

1. Select your **Directional Light** in the level.
2. In **Details → Actor → Tags**, add: `DeepTimeSun`
3. The subsystem lazy-finds this light and drives its Pitch rotation.

## Step 2: Create MPC_DeepTime

1. Content Browser → **Right-click → Materials → Material Parameter Collection**.
2. Name it `MPC_DeepTime`.
3. Add three **Scalar Parameters**:
   - `SunAngle` (0-360, degrees)
   - `SeasonAlpha` (0-1, seasonal position)
   - `TimeScale` (1-36525, current acceleration)

## Step 3: Use SeasonAlpha in Materials

In your landscape material, use a **Collection Parameter** node referencing `MPC_DeepTime`:

```
SeasonAlpha → Lerp
  A: Spring/Summer Color (warm greens)
  B: Autumn/Winter Color (amber/white)
  Alpha: SeasonAlpha (0 = winter, 0.5 = summer)
```

## Velocity → Deep Time Reference

| Velocity | TimeScale | Rate |
|----------|-----------|------|
| 0% (still) | 1.0 | 1 day per minute |
| 25% | ~9,132 | ~25 years per minute |
| 50% | ~18,263 | ~50 years per minute |
| 75% | ~27,394 | ~75 years per minute |
| 100% (max) | 36,525 | 1 century per minute |

**Standing still halts Deep Time. Moving accelerates it.**

## Sun Cycle

| Day Fraction | Angle | Position |
|-------------|-------|----------|
| 0.0 | 0° | Midnight |
| 0.25 | 90° | Sunrise |
| 0.5 | 180° | Noon |
| 0.75 | 270° | Sunset |
