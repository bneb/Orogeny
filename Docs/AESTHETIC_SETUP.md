# Romantic Aesthetic: Post-Process Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** ARomanticAtmosphereDirector (Day 11)

## Architecture

```
ARomanticAtmosphereDirector
  └─ UPostProcessComponent (Root)
       ├─ bUnbound = true          (applies to entire world)
       ├─ Priority = 100           (overrides all other PP volumes)
       ├─ Contrast = 1.25          (stark silhouettes)
       ├─ Saturation = 0.85        (muted, solemn)
       ├─ Shadows → cold blue      (brooding depth)
       ├─ Highlights → warm orange (god-rays)
       ├─ Vignette = 0.8           (heavy edge darkening)
       └─ FilmGrain = 0.15         (canvas texture)
```

**Zero GPU cost** — uses UE5's built-in filmic tonemapper (runs regardless).

## Step 1: Place in World

1. Drag `ARomanticAtmosphereDirector` into `MAP_Orogeny_Testbed`.
2. Position doesn't matter — `bUnbound = true` applies globally.
3. The entire world immediately adopts the Romantic profile.

## Step 2: Optimize the Directional Light

For maximum dramatic effect with the split toning:

1. Select the **Directional Light** in the level.
2. Set **Rotation Pitch** to approximately **-15°** (low angle).
3. This creates long shadows and intersects the Volumetric Fog, causing:
   - **Shadows** → cold blue tones (the brooding sky)
   - **Highlights** → warm orange god-rays piercing through fog

## Color Profile Reference

| Parameter | Value | Visual Effect |
|-----------|-------|---------------|
| Contrast | 1.25 | Crushed midtones, stark silhouettes |
| Saturation | 0.85 | Muted, solemn palette |
| Shadow Gamma | (0.9, 0.9, 1.1) | Cold blue depth |
| Highlight Gamma | (1.1, 1.05, 0.9) | Warm orange peaks |
| Vignette | 0.8 | Heavy edge darkening |
| Film Grain | 0.15 | Subtle canvas texture |

## Design Intent

> *Caspar David Friedrich: stark contrasts, brooding skies, heavy silhouettes, profound isolation.*

The aesthetic is **hardcoded** — not exposed as `EditAnywhere`. This prevents accidental drift from the art direction. Priority 100 ensures no level designer's PP volume can override it.
