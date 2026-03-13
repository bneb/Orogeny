# Game Loop: Win/Loss State Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** AOrogenyGameModeBase (Day 12)

## Architecture

```
AOrogenyGameModeBase
  ├─ EOrogenyGameState: Playing → Victory | Defeat
  ├─ RequiredSurvivalTime = 900s (15 min)
  ├─ MaxStormExposure = 10s
  ├─ CalculateExposureDelta()   (pure math — recovery mechanic)
  ├─ EvaluateGameState()        (pure math — priority: Defeat > Victory)
  ├─ HandleVictory() → DisableInput + OnVictory (Blueprint)
  └─ HandleDefeat()  → DisableInput + OnDefeat (Blueprint)
```

**No physics overlaps** — pure distance math vs. StormCore radius.

## Step 1: Create Blueprint GameMode

1. Content Browser → **Right-click → Blueprint Class → AOrogenyGameModeBase**.
2. Name it `BP_OrogenyGameMode`.
3. Open the Blueprint.

## Step 2: Implement OnVictory Event

```
Event OnVictory
  → Create Widget (WBP_VictoryScreen)
  → Add to Viewport
  → Play Sound 2D (S_VictoryMusic)
  → Delay (3.0s)
  → Open Level (MainMenu) or Quit
```

## Step 3: Implement OnDefeat Event

```
Event OnDefeat
  → Create Widget (WBP_DefeatScreen)
  → Add to Viewport
  → Play Sound 2D (S_DefeatMusic)
  → Delay (3.0s)
  → Open Level (Restart or MainMenu)
```

## Step 4: Assign in World Settings

1. Open **World Settings** for your level.
2. Set **GameMode Override** = `BP_OrogenyGameMode`.

## Exposure Recovery Reference

```
Inside storm:  Exposure += DeltaTime (danger builds)
Outside storm: Exposure -= DeltaTime (recovery!)
Clamped to [0, MaxStormExposure]
```

| Time in Storm | Time Outside | Net Exposure | Status |
|---------------|-------------|-------------|--------|
| 5s | 0s | 5.0 | Playing |
| 5s | 3s | 2.0 | Playing (recovered!) |
| 10s | 0s | 10.0 | **DEFEAT** |

## State Priority

**Defeat ALWAYS overrides Victory.** If the player survives 15 minutes while maxing storm exposure in the same frame, they lose. The storm claims all.
