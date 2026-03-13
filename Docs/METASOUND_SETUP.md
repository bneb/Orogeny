# MetaSound Director: Setup Guide

> **Audience:** Audio Engineer / Sound Designer
> **System:** AMetaSoundDirector (Sprint 10)

## What It Does

The `AMetaSoundDirector` reads game state (altitude, ecosystem health, Deep Time scale) and sends normalized float parameters to a MetaSoundSource asset. **You do NOT need to poll game state from Blueprints.** All values arrive as float inputs in your MetaSound graph.

## Step 1: Create the MetaSoundSource

1. **Content Browser** → Right-click → **Sounds** → **MetaSoundSource** → Name: `MS_OrogenyAmbience`.
2. Open the MetaSound graph editor.

## Step 2: Add Float Inputs (Must Match C++ Exactly)

Add three **Float** inputs to the MetaSound graph:

| Input Name | Range | Driven By |
|-----------|-------|-----------|
| `WindIntensity` | [0, 1] | Player altitude (valley=0, peak=1) |
| `FaunaVolume` | [0, 1] | Ecosystem health (dead=0, alive=1) |
| `TimePitch` | [1, 3] | Deep Time scale (log-compressed) |

> ⚠️ Names are **case-sensitive** and must match exactly.

## Step 3: Design the Audio Graph

### Wind Layer
```
WindIntensity → Gain → Wind Noise Generator
                     → Low-Pass Filter (cutoff scales with intensity)
```
- At 0.0: silence
- At 1.0: howling alpine gale

### Fauna Layer
```
FaunaVolume → Gain → Bird/Insect Sample Players (looping, randomized)
```
- At 0.0: dead silence (Blight has consumed the forest)
- At 1.0: rich natural chorus

### Time Compression (Symphonic Compression)
```
TimePitch → Pitch Shift node → applied to all layers
          → also drive Playback Speed on sample players
```
- At 1.0: normal speed, natural tempo
- At 3.0: accelerated, eerie time-lapse effect (centuries passing)

## Step 4: Assign to Director

1. Drag an `AMetaSoundDirector` into the level.
2. Select it → Details panel → **MainAudioComponent** → **Sound** → `MS_OrogenyAmbience`.
3. The Director will auto-play on BeginPlay and tick at 10Hz.

## Step 5: Verify in PIE

1. Play in Editor.
2. Select the MetaSoundDirector in World Outliner.
3. Observe in Details panel:
   - `CurrentWindIntensity` changes as you move up/down
   - `CurrentFaunaVolume` reflects ecosystem health
   - `CurrentTimePitch` scales with Deep Time acceleration

## Performance Notes (780M iGPU)

- Director ticks at **10Hz** — negligible CPU cost
- MetaSound DSP runs on the audio thread, not game thread
- Keep total voices under 32 for iGPU targets
