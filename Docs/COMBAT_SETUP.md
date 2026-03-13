# Combat: Orographic Lift Setup Guide

> **Audience:** Tech Artist / Animator
> **System:** UOrographicLiftComponent + ASupercell_Actor (Day 9)

## Architecture Overview

```
ATitanCharacter
  └─ UOrographicLiftComponent
       ├─ LiftRadius = 30000 cm (300m AOE)
       ├─ SpeedDamage = 15.0
       └─ OpacityDamage = 0.25

ASupercell_Actor
  ├─ MovementSpeed  [50 → floor 10]
  ├─ CloudOpacity   [1.0 → floor 0.2]
  └─ WeakenStorm()  (floor-clamped)

UAnimNotify_ExecuteLift
  └─ Fires on montage impact frame → OrographicLiftComponent.ExecuteLift()
```

## Step 1: Add the Anim Notify to the Thrust Montage

1. Open the **Thrust Animation Montage** (the un-cancelable Day 4 action).
2. In the **Notifies** track, right-click → **Add Notify → Execute Orographic Lift**.
3. Drag it to the **impact frame** (the moment the Titan's hands slam down).
4. The notify will fire `ExecuteLift()` → sphere overlap → WeakenStorm on all Supercells in range.

## Step 2: Bind CloudOpacity to Volumetric Cloud Material

In the **BP_Supercell** Blueprint, add a **Tick** event:

```
Event Tick
  → Get CloudOpacity
  → Create Dynamic Material Instance (CloudVisuals material, slot 0)
  → Set Scalar Parameter Value ("Opacity", CloudOpacity)
```

Or for better performance, use a **Timeline** or **Material Parameter Collection**:

```
Event Tick
  → Set MPC Scalar ("StormOpacity", CloudOpacity)
```

Then in the Volumetric Cloud material, read `StormOpacity` from the MPC to control extinction/density.

## Damage Math Reference

```
WeakenStorm(SpeedReduction, OpacityReduction):
  MovementSpeed = Max(Speed - SpeedReduction, 10.0)
  CloudOpacity  = Max(Opacity - OpacityReduction, 0.2)
```

| Hit # | Speed | Opacity | Notes |
|-------|-------|---------|-------|
| 0 (base) | 50.0 | 1.00 | Full strength |
| 1 | 35.0 | 0.75 | First lift |
| 2 | 20.0 | 0.50 | Weakening |
| 3 | 10.0 | 0.25 | Speed floors |
| 4+ | 10.0 | 0.20 | Both floored — lingering threat |

## Design Intent

The Supercell **never fully stops or disappears**:
- **Min speed = 10 cm/s** — always creeping, always a threat
- **Min opacity = 0.2** — always visible, always menacing
- Takes **~3 hits** to reach floors from full strength
