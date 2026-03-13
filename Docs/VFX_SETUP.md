# Niagara VFX: Setup Guide

> **Audience:** VFX Artist
> **System:** UTitanVFXComponent (Sprint 13)
> **Budget:** AMD 780M iGPU — total alive particles < 2,000

## What It Does

The `UTitanVFXComponent` manages three particle layers driven by C++ game state. **You design the visual look — the code handles all parameter scaling.**

## Step 1: Create NS_FootstepDust

1. **Content Browser** → Right-click → **Niagara System** → **New system from template**.
2. Select **Simple Sprite Burst** → Name: `NS_FootstepDust`.
3. Open the system:

### Required Setup
- Add a **User Parameter** → Float → Name: `SpawnRate` (exactly this name)
- Set the **Spawn Rate** module to read from `SpawnRate` parameter
- **Particle Lifetime**: 0.3-0.5s (short, ephemeral)
- **Initial Size**: 10-30 cm (small dust puffs)
- **Color**: Warm earth tones (match terrain GrassColor/RockColor)
- **Velocity**: Slightly upward + random spread
- **Loop**: Set system to **Looping**

### Budget
- Max alive at any time: ~250 particles
- SpawnRate capped at 500/s by C++

## Step 2: Create NS_SubductionDebris

1. Create another Niagara system → Name: `NS_SubductionDebris`.
2. Select **Simple Mesh Burst** template.

### Required Setup
- Add a **User Parameter** → Int32 → Name: `BurstCount`
- Set **Burst Spawn** to read from `BurstCount`
- **Mesh**: Use Engine Cube or small rock mesh
- **Initial Velocity**: Explosive outward (radial burst)
- **Gravity**: Enable (rocks fall after burst)
- **Particle Lifetime**: 2-3s
- **Loop**: OFF — this is a one-shot, auto-destroy

### Budget
- Max burst: 1000 particles (one-shot, decays in 2-3s)

## Step 3: Create NS_BlightSmoke (Optional)

1. Create: `NS_BlightSmoke`.
2. Use **Simple Sprite Burst** template.

### Required Setup
- Add a **User Parameter** → Float → Name: `SmokeOpacity`
- Drive sprite alpha from `SmokeOpacity`
- **Color**: Industrial amber/brown (match BlightedSkyColor)
- **Size**: Large (100-300 cm) — volumetric haze
- **Velocity**: Slow lateral drift
- **Loop**: Looping

## Step 4: Assign to Component

1. Open your Titan Character Blueprint.
2. Add Component → **TitanVFXComponent**.
3. In Details panel, set:
   - **Footstep Dust System** = `NS_FootstepDust`
   - **Subduction Debris System** = `NS_SubductionDebris`
   - **Blight Smoke System** = `NS_BlightSmoke`

## Step 5: Verify in PIE

1. Play in Editor → select the TitanVFXComponent.
2. **Walk** → dust puffs appear, scale with speed.
3. **Subduction** → debris burst on depth change.
4. Details panel shows `CurrentDustRate` and `CurrentSmokeOpacity`.

## Performance Notes (780M iGPU)

- Component ticks at **20Hz** — parameter updates, not per-frame
- Dust: ~250 alive max (short lifetime + capped spawn)
- Debris: one-shot burst, auto-destroys after 3s
- GPU Sim preferred over CPU Sim for sprite particles
- Use `stat niagara` to monitor live particle counts
