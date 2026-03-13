# Triplanar Material: Setup Guide

> **Audience:** Tech Artist / Material Designer
> **System:** ATerrainChunk MID Pipeline (Sprint 7)

## Why Triplanar?

Standard UV mapping stretches badly on procedural meshes because vertex positions are noise-driven. **WorldAlignedTexture** projects textures from 3 axes simultaneously, eliminating stretching entirely.

## Step 1: Create M_ProceduralTerrain

1. **Content Browser** → Right-click → **Material** → Name: `M_ProceduralTerrain`.
2. Set **Material Domain** = Surface, **Blend Mode** = Opaque, **Shading Model** = Default Lit.

## Step 2: Triplanar Texture Setup (DO NOT use TextureCoordinate nodes)

For each texture layer (Grass, Rock, Snow):

1. Add a **WorldAlignedTexture** node.
2. Connect your texture (e.g., `T_Grass_BC`) to the Texture input.
3. Set **TextureSize** to `(512, 512)` for world-space tiling (adjust for density).
4. For normals: use **WorldAlignedNormal** with the corresponding normal map.

## Step 3: Biome Blending (Mirrors C++ CalculateBiomeWeights)

### Rock/Grass Blend (Slope-based)
```
VertexNormalWS → BreakOutFloat3 → Z component
→ 1-x (invert: Z=1 is flat, we want slope)
→ Multiply by BlendSharpness (5.0)
→ Saturate
= RockWeight
```

### Snow Blend (Altitude-based)
```
AbsoluteWorldPosition → BreakOutFloat3 → Z component
→ Subtract SnowLineZ (25000)
→ Divide by (SnowLineZ * 0.2)
→ Multiply by BlendSharpness (5.0)
→ Saturate
= SnowAlpha

SnowWeight = SnowAlpha × (1 - RockWeight)    // Rock override
```

### Final Lerp Chain
```
Lerp(GrassColor, RockColor, RockWeight) → Result1
Lerp(Result1, SnowColor, SnowWeight) → Final BaseColor
```

Apply the same chain to Normal and Roughness outputs.

## Step 4: Seasonal Tinting (Deep Time Integration)

1. Add a **MaterialParameterCollection** node → select `MPC_DeepTime`.
2. Read **SeasonAlpha** (0 = Spring, 0.5 = Summer, 1.0 = Autumn/Winter).
3. Lerp the Grass layer:
```
Lerp(SpringGreen, AutumnGold, SeasonAlpha) → GrassColor
```

## Step 5: Blight Corruption Parameter

1. Add a **ScalarParameter** named `BlightCorruption` (default 0.0).
2. Lerp the entire output toward a rust/industrial color:
```
Lerp(FinalColor, BlightRustColor, BlightCorruption)
```
3. This is driven per-chunk by `UpdateChunkMaterialData()` from C++.

## Step 6: Assign to Chunks

In the **PangeaManagerComponent** or a Blueprint, set `BaseTerrainMaterial` on the chunk class default:
1. Select the **ATerrainChunk** Blueprint (or C++ default).
2. Set **Base Terrain Material** = `M_ProceduralTerrain`.
3. The MID is created automatically in `GenerateMesh()`.

## Parameter Reference

| Material Parameter | Source | Range |
|-------------------|--------|-------|
| BlightCorruption | C++ `UpdateChunkMaterialData()` | [0, 1] |
| SeasonAlpha | MPC_DeepTime | [0, 1] |
| SnowLineZ | Hardcoded or MPC | 25000 cm |

## Performance Notes (780M iGPU)

- Triplanar = 3 texture samples per layer × 3 layers = 9 samples. Acceptable.
- Use **Texture Arrays** if sample count becomes a bottleneck.
- World-aligned projection avoids UV computation overhead.
