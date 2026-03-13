# Ecosystem Armor: Procedural Flora Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** UEcosystemArmorComponent (Sprint 2)

## Architecture

```
UEcosystemArmorComponent (USceneComponent)
  ├─ FloraHISM (UHierarchicalInstancedStaticMeshComponent)
  ├─ IsValidGrowthLocation() → slope + altitude validation
  ├─ CalculateHealth() → normalized [0, 1]
  └─ Growth Tick (every 30 Deep Time days)
       └─ Raycast → Validate → AddInstance
```

## Step 1: Assign Flora Mesh

1. Open the **TitanCharacter Blueprint**.
2. Select the `EcosystemArmorComponent` → `FloraHISM` child.
3. In Details → **Static Mesh**, assign a low-poly pine tree (e.g., `SM_Pine_LowPoly`).
4. Ensure Nanite is enabled on the mesh for batched rendering.

## Step 2: Configure Titan Mesh Collision

The growth system raycasts downward onto the Titan's skeletal mesh. For this to work:

1. Select the Titan's **Skeletal Mesh Component**.
2. In **Collision → Collision Presets**, set to `BlockAll` or a custom profile that blocks `Visibility`.
3. Enable **Complex Collision as Simple** if the mesh has no physics asset.

## Step 3: Tune Growth Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| MaxSlopeAngle | 45° | Trees won't grow on cliffs steeper than this |
| MinGrowthAltitude | 1000 | Below = too low (submerged) |
| MaxGrowthAltitude | 25000 | Above = snow line (barren) |
| MaxInstances | 50,000 | Forest capacity = max health |

## Step 4: Reading Ecosystem Health

`EcosystemHealth` is a normalized float [0, 1]:
- **1.0** = forest fully grown (50,000 trees) = maximum armor
- **0.5** = half depleted
- **0.0** = barren mountain = vulnerable

Use this in materials via Blueprint or MPC to drive visual feedback (e.g., Titan surface color shifts from lush green to grey rock as health drops).
