# Iron Blight: Cellular Automata Setup Guide

> **Audience:** Tech Artist / Level Designer
> **System:** AIronBlightSeed + FCellularBlightGrid (Sprint 3)

## Architecture

```
AIronBlightSeed (AActor)
  ├─ FCellularBlightGrid (pure C++ — no UObject)
  │    ├─ GetNeighbors() → 4 cardinal directions
  │    └─ CalculateNextGeneration() → snapshot-based spread
  ├─ BlightScaffoldingHISM → visual scaffolding meshes
  └─ Deep Time sync → expands once per in-game year
       ├─ Raycast → place scaffolding on Titan surface
       └─ SphereOverlap → destroy Ecosystem flora
```

## Step 1: Assign Scaffolding Mesh

1. Place an `AIronBlightSeed` actor near the Titan in the level.
2. Select the actor → `BlightScaffoldingHISM` component.
3. In Details → **Static Mesh**, assign an industrial scaffolding mesh (e.g., `SM_IronScaffold_Rusted`).
4. Enable Nanite on the mesh for batched rendering.

## Step 2: Tune Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| GridCellSize | 500 cm | Spacing between blight nodes. Lower = denser spread |
| SpreadProbability | 0.25 | 25% chance per neighbor per year. Higher = more aggressive |
| TreeDestructionRadius | 600 cm | Overlap radius for killing ecosystem flora |

## Step 3: Understand the Spread

The Blight uses **Von Neumann Cellular Automata** (4-directional, no diagonals):

```
Generation 0:     Generation 1:     Generation 2:
                       X                  X
    X              X X X              X X X X X
                       X               X X X
                                         X
```

Each gen, each corrupted cell rolls `SpreadProbability` per clean neighbor. At 25%, expect roughly 1 new cell per corrupted cell per year.

## Step 4: Visual Density

`GridCellSize` controls how dense the scaffolding appears:
- **250 cm**: Dense, claustrophobic industrial hellscape
- **500 cm** (default): Natural parasitic spread
- **1000 cm**: Sparse, creeping infestation
