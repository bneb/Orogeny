# Terrain Deformation: Trench Mask Setup Guide

> **Audience:** Tech Artist / Material Designer
> **System:** UTectonicTrenchComponent (Day 6)

## Architecture Overview

```
TitanCharacter
  └─ UTectonicTrenchComponent
       ├─ TrenchMaskRT      (R16f, 2048x2048, persistent)
       ├─ CurrentTrenchUV    (FVector2D, updated per tick)
       └─ bShouldPaintThisFrame (bool, velocity-gated)
```

- **C++ owns** the Render Target lifecycle, format, and UV math.
- **Blueprint paints** depth stamps onto `TrenchMaskRT` using `UCanvas` each frame when `bShouldPaintThisFrame == true`.
- **Landscape Material reads** `TrenchMaskRT` and applies it to **World Position Offset**.

## Step 1: Blueprint — Paint the Trench Mask

1. In the **TitanCharacter Blueprint**, override `Event Tick`.
2. Get a reference to `TrenchComponent`.
3. Check `bShouldPaintThisFrame` — skip if `false`.
4. Call `Begin Draw Canvas to Render Target` with `TrenchMaskRT`.
5. Draw a circular brush (e.g., 64x64 soft circle texture) at `CurrentTrenchUV * RT Resolution`.
6. Use a dark color (e.g., `R=0.0` for full depression, `R=0.5` for half).
7. Call `End Draw Canvas to Render Target`.

## Step 2: Landscape Material — Read the Mask

In your **Landscape Material (M_Landscape)**:

```
[TextureSampleParameter2D: "TrenchMask"]
    ├─ Texture = TrenchMaskRT (set via MID in Blueprint)
    └─ Sampler Type = Linear Color
         │
         ▼
    [OneMinus]   ← Invert: white (1.0) = undisturbed, black (0.0) = full depth
         │
         ▼
    [Multiply]
         ├─ A = Inverted mask value
         └─ B = ScalarParameter "TrenchDepth" = -500.0
              │
              ▼
         [AppendVector]
              ├─ XY = (0, 0)
              └─ Z = Depth result
                   │
                   ▼
              [World Position Offset]
```

### Key Settings

| Parameter | Value | Reason |
|-----------|-------|--------|
| `TrenchDepth` | `-500.0` | Negative = pushes terrain downward. Adjust for scale. |
| UV Tiling | `1.0` | The RT already maps 1:1 to `WorldSize`. No tiling needed. |
| Sampler | `Clamp` | Prevents tiling artifacts at terrain edges. |

## Step 3: Material Instance — Wire the RT at Runtime

In the **Level Blueprint** or a **Manager Actor**:

1. Create a **Dynamic Material Instance** from `M_Landscape`.
2. Call `Set Texture Parameter Value` with name `"TrenchMask"` and value `TrenchComponent->TrenchMaskRT`.
3. Apply the MID to the Landscape actor.

## UV Mapping Reference

The component uses this mapping:

```
UV = (WorldPosition.XY / WorldSize) + 0.5
```

| World Position | UV Result |
|---------------|-----------|
| `(0, 0, 0)` | `(0.5, 0.5)` — Center |
| `(50000, 50000, 0)` | `(1.0, 1.0)` — Max edge |
| `(-50000, -50000, 0)` | `(0.0, 0.0)` — Min edge |

The Landscape Material must use the **same UV mapping** when sampling:

```hlsl
// In Custom HLSL node (or Material Function):
float2 TrenchUV = (WorldPosition.xy / WorldSize) + 0.5;
```

## Performance Notes

- **R16f format** — single 16-bit float channel. 4x smaller than RGBA8, 8x smaller than RGBA16f.
- **Velocity gating** — no GPU draw dispatch when the Titan is stationary.
- **2048x2048 default** — covers 100,000cm (1km) at ~49cm/pixel resolution.
- VRAM cost: `2048 * 2048 * 2 bytes = ~8MB`. Negligible on the 780M.
