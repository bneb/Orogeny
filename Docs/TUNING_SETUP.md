# Difficulty Tuning: Setup Guide

> **Audience:** Game Designer
> **System:** UOrogenyDifficultyPreset + FTuningMath (Sprint 15)

## What It Does

Difficulty presets are **Data Assets** — no code changes needed to add or tweak difficulties. Create them in the Content Browser, adjust multipliers, assign to the GameMode.

## Step 1: Create Difficulty Data Assets

### DA_Diff_Story (Easy)
1. **Content Browser** → Right-click → **Miscellaneous** → **Data Asset**.
2. Select **OrogenyDifficultyPreset** as the class.
3. Name: `DA_Diff_Story`.
4. Set:
   - **PresetName**: `Story`
   - **BlightSpreadMultiplier**: `0.5` (blight spreads half as fast)
   - **EcosystemHealMultiplier**: `2.0` (healing twice as fast)
   - **TargetSurvivalCenturies**: `5.0` (shorter game)

### DA_Diff_Survival (Normal)
1. Create: `DA_Diff_Survival`.
2. Set:
   - **PresetName**: `Survival`
   - **BlightSpreadMultiplier**: `1.0` (default)
   - **EcosystemHealMultiplier**: `1.0` (default)
   - **TargetSurvivalCenturies**: `10.0` (1,000 years)

### DA_Diff_Endless (Sandbox)
1. Create: `DA_Diff_Endless`.
2. Set:
   - **PresetName**: `Endless`
   - **BlightSpreadMultiplier**: `1.5` (blight keeps pressure up)
   - **EcosystemHealMultiplier**: `1.0` (default)
   - **TargetSurvivalCenturies**: `0.0` (no victory — play forever)

## Step 2: Assign to GameMode

1. Open `BP_OrogenyGameMode` (or create from `AOrogenyGameModeBase`).
2. In Details → **Difficulty** → **Active Difficulty**.
3. Select the desired Data Asset (e.g., `DA_Diff_Survival`).

## Step 3: Main Menu Integration (Future)

Wire a difficulty selector in the Main Menu that sets `ActiveDifficulty` before starting the game. The GameMode reads it on `BeginPlay`.

## How the Math Works

```
Base Blight Probability × BlightSpreadMultiplier → Clamped [0.01, 1.0]
                                            (blight never fully stops)

Base Healing Rate × EcosystemHealMultiplier → Clamped [0.0, 1000.0]

TargetSurvivalCenturies == 0 → Endless Mode (victory disabled)
```

## Verify in PIE

1. Assign `DA_Diff_Story` → blight barely spreads, healing strong.
2. Assign `DA_Diff_Endless` → no victory condition, game runs forever.
3. Check Output Log for difficulty preset announcement on BeginPlay.
