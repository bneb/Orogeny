# MAP_Orogeny_Testbed — Level Setup Guide

This document provides exact instructions for creating the Day 1 test level in the Unreal Editor.

## Prerequisites

- Project compiles successfully (zero errors)
- All automation tests pass in Session Frontend

## Step 1: Create the Level

1. **File → New Level → Empty Level**
2. **File → Save Current Level As…** → `Content/Maps/MAP_Orogeny_Testbed`

## Step 2: Ground Plane

1. **Place Actors → Shapes → Plane**
2. Transform:
   - Location: `(0, 0, 0)`
   - Scale: `(100, 100, 1)` — creates a 10,000×10,000 unit ground
3. Apply a neutral gray material (or `M_Basic_Floor` from Starter Content)

## Step 3: Directional Light (God Rays)

1. **Place Actors → Lights → Directional Light**
2. Properties:
   - **Intensity**: `10 lux`
   - **Rotation**: `(-45, -30, 0)` — dramatic low-angle for god-rays through fog
   - **Light Color**: Warm white `(255, 244, 229)` — late afternoon sun
   - **Atmosphere Sun Light**: ✅ Enabled
   - **Cast Volumetric Shadow**: ✅ Enabled
   - **Volumetric Scattering Intensity**: `1.5`
   - **Source Angle**: `2.0` — soft penumbras for painterly shadows

## Step 4: Sky Light

1. **Place Actors → Lights → Sky Light**
2. Properties:
   - **Source Type**: `SLS Captured Scene` (real-time capture)
   - **Intensity Scale**: `1.0`
   - **Real Time Capture**: ✅ Enabled
   - **Lower Hemisphere is Solid Color**: ✅ Enabled
   - **Lower Hemisphere Color**: Dark earth `(30, 25, 20)`

## Step 5: Exponential Height Fog

1. **Place Actors → Fog → Exponential Height Fog**
2. Properties:
   - **Fog Density**: `0.02`
   - **Fog Height Falloff**: `0.2` — slow falloff for deep atmospheric haze
   - **Fog Inscattering Color**: Warm gray `(180, 170, 160)`
   - **Volumetric Fog**: ✅ **Enabled** (critical)
   - **Volumetric Fog Scattering Distribution**: `0.9`
   - **Volumetric Fog Albedo**: `(220, 215, 200)` — muted warm tone
   - **Volumetric Fog Emissive**: `(0, 0, 0)`
   - **Volumetric Fog Extinction Scale**: `1.0`
   - **View Distance**: `50000`

## Step 6: Post Process Volume (Romantic Grading)

1. **Place Actors → Volumes → Post Process Volume**
2. Properties:
   - **Infinite Extent (Unbound)**: ✅ Enabled
   - **Priority**: `0`

### Bloom
   - **Method**: `Standard`
   - **Intensity**: `0.8`
   - **Threshold**: `-1` (bloom everything subtly)

### Exposure
   - **Metering Mode**: `Manual`
   - **Exposure Compensation**: `1.0`

### Color Grading (Day 11 will refine)
   - **Global Saturation**: `(0.9, 0.85, 0.9, 1.0)` — slight desaturation
   - **Global Contrast**: `(1.1, 1.1, 1.1, 1.0)` — mild contrast boost
   - **Shadows Gain**: `(1.0, 0.95, 0.85, 1.0)` — warm shadow push

### Ambient Occlusion
   - **Intensity**: `0.5`
   - **Radius**: `200.0`

### Motion Blur
   - **Amount**: `0.3`
   - **Max**: `3.0`

## Step 7: World Settings

1. Open **World Settings** panel
2. **GameMode Override**: `OrogenyGameModeBase`
3. **Default Pawn Class**: Should auto-inherit `ATitanCharacter` from GameMode

## Step 8: Validate

1. Press **Play (PIE)** — confirm:
   - Volumetric fog renders with visible god-rays from the directional light
   - The Titan character spawns at origin
   - Camera is positioned far behind at ~2500 units

2. Open Console (`~`):
   ```
   stat unit
   stat gpu
   ```
   - Frame time should be **well under 16.6ms** on this blank level
   - Base pass should be **< 4ms**

3. Open **Session Frontend → Automation**:
   - Filter: `Orogeny.Config`
   - Run all → All tests should be **Green** ✅
