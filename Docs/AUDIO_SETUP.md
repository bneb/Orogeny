# Tectonic Audio: MetaSound & Sub-Bass Setup Guide

> **Audience:** Tech Artist / Audio Designer
> **System:** UTectonicAudioComponent + UAnimNotify_TectonicStep (Day 10)

## Prerequisites

Ensure the **MetaSound** plugin is enabled:
1. Edit → Plugins → search `MetaSound`
2. Enable **MetaSounds** (exact plugin name: `MetaSound`)
3. Restart the editor

## Architecture Overview

```
ATitanCharacter
  └─ UTectonicAudioComponent
       ├─ MovementLoopSound    (MetaSound Source, assigned in Editor)
       ├─ MovementAudioComp    (spawned UAudioComponent)
       ├─ SpeedParameterName   = "SpeedAlpha"
       └─ CurrentSpeedAlpha    [0, 1] (updated per tick)

UAnimNotify_TectonicStep
  └─ FootstepSound             (MetaSound for impact thud)
```

**Two audio layers:**
1. **Movement Loop** — continuous sub-bass rumble + grinding, scaled by velocity
2. **Footstep Impact** — per-step thud from the animation notify

## Step 1: Create the Movement Loop MetaSound

1. Content Browser → **Right-click → Sounds → MetaSound Source**.
2. Name it `MS_TectonicMovement`.
3. Open the MetaSound graph.

### Input Variables

| Name | Type | Range | Description |
|------|------|-------|-------------|
| `SpeedAlpha` | Float | [0, 1] | Normalized velocity from C++ |

### Sub-Bass Layer (20-40 Hz)

```
[Input: SpeedAlpha]
  → [Multiply × 20.0]     ← Maps [0,1] to [0, 20]
  → [Add + 20.0]           ← Range becomes [20, 40] Hz
  → [Sine LFO Frequency]
  → [Sine Oscillator]
  → [Multiply × SpeedAlpha]  ← Volume scales with speed
  → [Output: Left/Right]
```

### Grinding Rock Layer

```
[Input: SpeedAlpha]
  → [Multiply × Volume node]
  → [Wave Player: rock_grinding_loop.wav]
  → [Mix with Sub-Bass]
  → [Output: Left/Right]
```

## Step 2: Assign in Blueprint

1. Open the **TitanCharacter Blueprint**.
2. Select the `AudioComponent` (UTectonicAudioComponent).
3. Set **MovementLoopSound** = `MS_TectonicMovement`.
4. The component auto-manages playback: plays when moving, stops when stationary.

## Step 3: Footstep Impact Sound

1. Create a second MetaSound `MS_TectonicFootstep` — a short sub-bass thud.
2. Open the **Walk/Run Animation Montage**.
3. Find the `TectonicStep` notifies on each foot-down frame.
4. In the Details panel, set **FootstepSound** = `MS_TectonicFootstep`.

## Data Pipeline Reference

```
SpeedAlpha = Clamp(CurrentSpeed / MaxSpeed, 0, 1)
```

| Speed | Alpha | Sub-Bass Hz | Grinding Vol | Movement Loop |
|-------|-------|-------------|-------------|---------------|
| 0 | 0.00 | — | Silent | Stopped |
| 15 | 0.10 | 22 Hz | 10% | Playing |
| 75 | 0.50 | 30 Hz | 50% | Playing |
| 150 | 1.00 | 40 Hz | 100% | Playing |

The `SILENCE_THRESHOLD = 0.01` prevents inaudible playback from consuming audio resources.
