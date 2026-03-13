# Supercell (Antagonist): Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** ASupercell_Actor (Day 8)

## Architecture Overview

```
ASupercell_Actor
  ├─ USphereComponent "StormCore"       (Root, radius 20000cm / 200m)
  ├─ UVolumetricCloudComponent "CloudVisuals"  (attached to root)
  ├─ MovementSpeed = 50 cm/s            (1/3rd Titan speed — slow dread)
  └─ TargetActor                        (set from Level Blueprint)
```

The Supercell uses pure-math tracking — no NavMesh, no AI Controller.

## Step 1: Create the Blueprint

1. In Content Browser: **Right-click → Blueprint Class → ASupercell_Actor**.
2. Name it `BP_Supercell`.
3. Open `BP_Supercell` and select the `CloudVisuals` component.
4. Assign your **Volumetric Cloud Material** (e.g., `M_StormCloud`).
5. Adjust cloud layer altitude, scale, and density to taste.

## Step 2: Set the Target (Level Blueprint)

In your **Level Blueprint**:

```
Event BeginPlay
  → Get Player Pawn (0)
  → Get All Actors Of Class (ASupercell_Actor)
  → For Each → Set TargetActor = Player Pawn
```

Or in the Supercell Blueprint `Event BeginPlay`:

```
Event BeginPlay
  → Get Player Pawn (0)
  → Set TargetActor
```

## Step 3: Place in World

1. Drag `BP_Supercell` into the level.
2. Position it far from the player start (e.g., 50,000+ cm away).
3. The storm will slowly creep toward the player at 50 cm/s.

## Tracking Math Reference

```
Direction = Normalize(Target - Current)
StepSize = Speed × DeltaTime
if Distance < StepSize → snap to Target (anti-jitter)
else → Current + Direction × StepSize
```

| Scenario | Speed | Distance | Result |
|----------|-------|----------|--------|
| Normal tracking | 50 | 1000 | Moves 50 cm/frame toward target |
| Close to target | 50 | 10 | Snaps exactly to target position |
| Diagonal | 50 | 141 | Moves exactly 50 cm (normalized) |

## Tuning Tips

| Property | Default | Effect |
|----------|---------|--------|
| `MovementSpeed` | 50 | Increase for more aggressive storm. Max ~150 = Titan speed. |
| `StormCore` radius | 20000 | Visual/gameplay overlap sphere. Increase for wider "storm area". |
| Cloud material | — | Use dark, dense volumetric materials for menacing appearance. |
