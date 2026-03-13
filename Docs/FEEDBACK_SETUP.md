# Tectonic Feedback: Setup Guide

> **Audience:** Tech Artist / Game Designer
> **System:** UTectonicFeedbackComponent (Sprint 11)

## What It Does

The `UTectonicFeedbackComponent` translates the Titan's physical state into **game feel**: camera shakes when walking, controller rumble during subduction, and a diegetic vignette when the ecosystem is dying.

## Step 1: Create Camera Shake Assets

### Footstep Shake
1. **Content Browser** → Right-click → **Blueprint Class** → search `CameraShakeBase` → Name: `CS_FootstepHeave`.
2. Open it → Set **Root Shake Pattern** to `PerlinNoiseCameraShakePattern`.
3. Configure:
   - **Location Z**: Amplitude = 3.0, Frequency = 2.0
   - **Rotation Pitch**: Amplitude = 0.3, Frequency = 1.5
   - **Duration**: 0 (looping — scale controls intensity)

### Subduction Shake
1. Create another: `CS_SubductionGrind`.
2. Configure:
   - **Location Z**: Amplitude = 8.0, Frequency = 15.0 (violent grinding)
   - **Rotation Roll**: Amplitude = 1.0, Frequency = 8.0
   - **Duration**: 0 (looping)

## Step 2: Create Force Feedback Asset

1. **Content Browser** → Right-click → **Miscellaneous** → **Force Feedback Effect** → Name: `FF_SubductionRumble`.
2. Add a channel entry:
   - **Affects Large Motor**: True (heavy bass rumble)
   - **Duration**: 2.0
   - Add curve key at (0.0, 0.8) and (2.0, 1.0) for ramping intensity.

## Step 3: Assign to Component

1. Open your Titan Character Blueprint.
2. Add Component → **TectonicFeedbackComponent**.
3. In Details panel, set:
   - **Footstep Shake Class** = `CS_FootstepHeave`
   - **Subduction Shake Class** = `CS_SubductionGrind`
   - **Subduction Rumble Asset** = `FF_SubductionRumble`

## Step 4: Health Vignette (Post-Process)

1. Add a **Post Process Volume** to the level (Infinite Extent = True).
2. In the PPV settings:
   - Set **Vignette Intensity** = 0 (will be driven by C++)
   - Set **Color Saturation** = 1.0 (will be desaturated at low health)
3. The component exposes `CurrentVignetteWeight` [0, 1]:
   - Map to VignetteIntensity via Blueprint or MPC
   - At 1.0: heavy vignette + desaturation (dying mountain)

## How the Math Works

```
Velocity Alpha → Dead Zone (< 0.1 = no shake)
               → Map [0.1, 1.0] to [0.1, 1.0] shake scale

Health → Critical Threshold (> 0.3 = no vignette)
       → Map [0.3, 0.0] to [0.0, 1.0] vignette weight

Subduction → State Gate (not transitioning = no rumble)
           → Map depth/maxDepth to [0.5, 1.0] intensity
```

## Performance Notes (780M iGPU)

- Component ticks at **20Hz** — smooth feedback, lean CPU
- Camera shakes run on the rendering thread via player controller
- Force feedback is controller-side, zero GPU cost
- Post-process vignette uses a single full-screen pass
