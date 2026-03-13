# Atmospheric Director: Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** AAtmosphericDirector (Sprint 8)

## What It Does

The `AAtmosphericDirector` hijacks `ASkyAtmosphere` and `AExponentialHeightFog` at runtime. **You do NOT animate these actors manually.** Deep Time drives the day/night cycle. Blight progression shifts the atmosphere from Romantic clarity to industrial suffocation.

## Step 1: Level Setup

1. Drag an **Exponential Height Fog** into `MAP_Orogeny_Testbed`.
   - Default density will be overridden — set it to anything.
2. Drag a **Sky Atmosphere** into the map.
3. Drag an **AAtmosphericDirector** into the map (C++ class).
   - It finds the above actors automatically via `GetAllActorsOfClass`.

## Step 2: Verify Runtime Control

1. Play in Editor (PIE).
2. Select the `AtmosphericDirector` in the World Outliner.
3. In the Details panel, observe:
   - **CurrentTimeOfDayAlpha** cycling [0, 1] as Deep Time advances.
   - **CurrentFogDensity** at 0.02 (clean air).
4. Change **DebugBlightRatio** to 0.5 → fog thickens, sky shifts amber.
5. Change **DebugBlightRatio** to 1.0 → full industrial smog.

## Step 3: Tuning Parameters

| Property | Default | Effect |
|----------|---------|--------|
| BaseFogDensity | 0.02 | Clean-air haze level |
| HealthySkyColor | (0.1, 0.2, 0.4) | Romantic blue/grey palette |
| BlightedSkyColor | (0.6, 0.3, 0.0) | Industrial amber corruption |
| DebugBlightRatio | -1.0 | Set ≥ 0 to override, -1 for automatic |

## Step 4: Volumetric Clouds (Optional Enhancement)

1. Drag a **VolumetricCloud** actor into the level.
2. The AtmosphericDirector does not yet drive clouds programmatically.
3. To integrate: modify the Director's Tick to read `CurrentTimeOfDayAlpha`
   and SetCloudOpacity on the cloud component.

## How the Math Works

```
Sun Angle → Triangle Wave → TimeOfDayAlpha [0, 1]
  0° = Midnight (0.0)
  90° = Dawn (0.5)
  180° = Noon (1.0)
  270° = Dusk (0.5)

Blight Ratio → Fog Density = Base × (1 + Ratio × 4)
  0.0 = 0.02 (clear)
  1.0 = 0.10 (choking)

Blight Ratio → Sky Color = Lerp(Blue, Amber, Ratio)
```

## Performance Notes (780M iGPU)

- Director ticks at **10Hz** (0.1s interval), not per-frame.
- Fog and sky updates are single-property sets — negligible cost.
- Volumetric Clouds are the heaviest item — test with `stat gpu`.
