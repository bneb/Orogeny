# Atmospheric Wake: MPC & Fog Displacement Setup Guide

> **Audience:** Tech Artist / VFX Designer
> **System:** UTectonicAtmosphereComponent (Day 7)

## Architecture Overview

```
TitanCharacter
  └─ UTectonicAtmosphereComponent (attached to Root)
       ├─ AtmosphereMPC          (assigned in Editor)
       ├─ DustWakeSystem         (spawned Niagara component)
       ├─ CurrentDisplacementRadius  (velocity-scaled, updated per tick)
       └─ CurrentWakeIntensity       ([0,1], updated per tick)
```

The component pushes two values into the MPC each tick:
- **TitanLocation** (Vector) — the Titan's world position
- **DisplacementRadius** (Scalar) — velocity-scaled radius `[5000, 10000]`

## Step 1: Create the Material Parameter Collection

1. In Content Browser: **Right-click → Materials → Material Parameter Collection**.
2. Name it `MPC_TitanAtmosphere`.
3. Add parameters:

| Type | Name | Default |
|------|------|---------|
| Vector | `TitanLocation` | `(0, 0, 0, 1)` |
| Scalar | `DisplacementRadius` | `5000.0` |

4. In the **TitanCharacter Blueprint**, select the `AtmosphereComponent`.
5. Set **AtmosphereMPC** to `MPC_TitanAtmosphere`.

## Step 2: Fog Displacement Material

In your **Exponential Height Fog** custom material (or post-process):

```
[MPC: TitanLocation]  ──────┐
                              ▼
[WorldPosition]  ──→  [Distance]  ──→  [SphereMask]
                                            │
[MPC: DisplacementRadius]  ────────→  [Radius Input]
                                            │
                                       [Hardness = 0.5]
                                            │
                                            ▼
                                       [OneMinus]
                                            │
                                            ▼
                                    [Multiply × FogDensity]
                                            │
                                            ▼
                                       [Opacity]
```

### SphereMask Node Setup

| Input | Source | Notes |
|-------|--------|-------|
| A | `WorldPosition` | Absolute World Position node |
| B | `MPC: TitanLocation` | Auto-updated by C++ each tick |
| Radius | `MPC: DisplacementRadius` | Scales with velocity |
| Hardness | `0.5` (scalar) | Soft falloff. Increase for sharper edge. |

The result: fog **clears around the Titan** as it moves, with the clearing radius expanding at higher speeds.

## Step 3: Niagara Dust Wake

1. Create a Niagara System `NS_DustWake` — dust particles that trail the Titan.
2. Add a **User.WakeIntensity** float parameter (range `[0, 1]`).
3. Bind particle spawn rate to `WakeIntensity`:
   - `SpawnRate = WakeIntensity * MaxParticlesPerSecond`
4. In the **TitanCharacter Blueprint**, set **DustWakeAsset** on the `AtmosphereComponent` to `NS_DustWake`.

## Displacement Math Reference

```
SpeedAlpha = Clamp(CurrentSpeed / MaxSpeed, 0, 1)
Radius = BaseRadius × (1.0 + SpeedAlpha × (Multiplier - 1.0))
```

| Speed | Alpha | Radius |
|-------|-------|--------|
| 0 (stationary) | 0.0 | 5,000 cm |
| 75 (half speed) | 0.5 | 7,500 cm |
| 150 (max speed) | 1.0 | 10,000 cm |
| 300 (overspeed) | 1.0 | 10,000 cm (clamped) |
