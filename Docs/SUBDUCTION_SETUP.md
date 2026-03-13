# Subduction Mechanics: Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** UTitanSubductionComponent (Sprint 4)

## Architecture

```
UTitanSubductionComponent (UActorComponent)
  ├─ ToggleSubduction() → flips target state
  ├─ CalculateSubductionZ() → FInterpConstantTo (linear, heavy)
  ├─ Tick: drive Z-axis toward target
  ├─ At full depth: CrushBlightAtDepth() → SphereOverlap
  └─ HealingMultiplier → accelerates Ecosystem regrowth
```

## Step 1: Camera Configuration

**Critical:** The camera SpringArm must NOT follow the Titan's Z offset during subduction, or it will clip into the ground.

1. Select the `CameraBoom` (SpringArmComponent) on the TitanCharacter.
2. Ensure **Inherit Pitch/Yaw/Roll** on the attachment is configured so the camera stays above ground.
3. Alternatively, set `bAbsoluteLocation = true` on the Z axis of the boom socket offset.

## Step 2: Particle Effects

During `bIsTransitioning`, trigger heavy dust/rock VFX:

1. Create a Niagara System: `NS_SubductionDust` — heavy rock debris, ground cracks.
2. In Blueprint, bind to `bIsTransitioning`:
   - `true` → Activate system
   - `false` → Deactivate system

## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| SubductionDepth | -15000 cm | How deep the Titan sinks (150m) |
| TransitionSpeed | 2000 cm/s | Linear descent speed (7.5s to full depth) |
| HealingMultiplier | 500× | Ecosystem growth acceleration while subducted |
| CrushRadius | 5000 cm | Radius for blight destruction at depth |

## Timing Math

| Speed | Full Depth Time | Description |
|-------|----------------|-------------|
| 1000 cm/s | 15.0 seconds | Slow, dramatic |
| 2000 cm/s | 7.5 seconds | Default — heavy but responsive |
| 4000 cm/s | 3.75 seconds | Fast, arcade feel |
