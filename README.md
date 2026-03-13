# 🏔️ Orogeny

**A game about being a mountain.**

You are the Titan — a humanoid colossus with the weight of geology. Trees grow on your shoulders. Centuries pass with each step. The Iron Blight devours your ecosystem while supercell storms rip across the infinite landscape. Survive long enough, and the mountain endures.

Built in **Unreal Engine 5.7** with strict **Test-Driven Development** — 139 passing headless tests across 27 suites.

---

## Architecture

```
Source/Orogeny/
├── Core
│   ├── DeepTimeSubsystem          → Geological chronology (centuries, day/night)
│   ├── OrogenyGameModeBase        → Win/loss loop, difficulty integration
│   └── TitanCharacter             → The player (humanoid mountain)
│
├── World
│   ├── ProceduralTerrainMath      → FBM + Ridged noise (infinite landscape)
│   ├── TerrainChunk               → Procedural mesh chunks
│   ├── PangeaManagerComponent     → Chunk streaming around the Titan
│   └── AtmosphericDirector        → Dynamic sky, fog, scattering
│
├── Life & Death
│   ├── EcosystemArmorComponent    → Flora HISM growth (the Titan's health)
│   ├── IronBlightSeed             → Antagonist spawner
│   └── CellularBlightGrid        → Cellular automata corruption
│
├── Combat
│   ├── TitanSubductionComponent   → Sink into the earth (defense)
│   └── Supercell_Actor            → Roaming storm antagonist
│
├── Feedback
│   ├── TectonicFeedbackComponent  → Camera shake, rumble, vignette
│   ├── TitanVFXComponent          → Niagara dust, debris, smoke
│   ├── MetaSoundDirector          → Adaptive wind, fauna, time-pitch
│   └── OrogenyHUDWidget           → Century, health, blight readout
│
├── Systems
│   ├── SaveLoadSubsystem          → Async save/load (GameInstance)
│   ├── OrogenyDifficultyPreset    → Data-driven difficulty (Story/Survival/Endless)
│   └── OrogenyPlayerController    → HUD lifecycle
│
├── Pure Math Libraries (TDD)
│   ├── FProceduralTerrainMath     → Noise, height, biome weights
│   ├── FAtmosphericMath           → Sky color, fog density
│   ├── FMetaSoundMath             → Wind, fauna, pitch scaling
│   ├── FFeedbackMath              → Shake, vignette, rumble curves
│   ├── FNiagaraMath               → Particle spawn/burst/opacity
│   ├── FUIMath                    → Century, health %, blight ratio
│   └── FTuningMath                → Difficulty multipliers
│
└── Tests/ (27 suites, 139 tests)
    ├── DeepTimeTest               ProceduralTerrainTest
    ├── EcosystemArmorTest         IronBlightTest
    ├── SubductionMechanicsTest    GameLoopStateTest
    ├── BiomeMaterialMathTest      AtmosphericMathTest
    ├── MetaSoundMathTest          FeedbackMathTest
    ├── CombatLiftTest             NiagaraMathTest
    ├── UIMathTest                 SerializationTest
    └── TuningMathTest
```

## Getting Started

### Prerequisites
- Unreal Engine 5.7+
- macOS or Windows

### Build
```bash
# macOS
"/Users/Shared/Epic Games/UE_5.7/Engine/Build/BatchFiles/RunUBT.sh" \
  OrogenyEditor Mac Development \
  -Project="$(pwd)/Orogeny.uproject"
```

### Run Tests
```bash
./RunTests.sh
# ✅ ALL 139 TESTS PASSED
```

### Complete the Game
Follow the step-by-step playbook:
```
📖 Docs/PLAYBOOK.md
```

## Documentation

| Guide | Audience | System |
|-------|----------|--------|
| [PLAYBOOK.md](Docs/PLAYBOOK.md) | Everyone | Master checklist |
| [DEEPTIME_SETUP.md](Docs/DEEPTIME_SETUP.md) | Tech Artist | Day/night, centuries |
| [TRIPLANAR_MATERIAL_SETUP.md](Docs/TRIPLANAR_MATERIAL_SETUP.md) | Tech Artist | Terrain material |
| [ATMOSPHERE_SETUP.md](Docs/ATMOSPHERE_SETUP.md) | Tech Artist | Sky & fog |
| [ATMOSPHERE_DIRECTOR_SETUP.md](Docs/ATMOSPHERE_DIRECTOR_SETUP.md) | Tech Artist | Dynamic atmosphere |
| [ECOSYSTEM_SETUP.md](Docs/ECOSYSTEM_SETUP.md) | Tech Artist | Flora HISM |
| [BLIGHT_SETUP.md](Docs/BLIGHT_SETUP.md) | Tech Artist | Corruption visuals |
| [SUBDUCTION_SETUP.md](Docs/SUBDUCTION_SETUP.md) | Designer | Defense mechanic |
| [COMBAT_SETUP.md](Docs/COMBAT_SETUP.md) | Designer | Lift/Weaken |
| [SUPERCELL_SETUP.md](Docs/SUPERCELL_SETUP.md) | Designer | Storm antagonist |
| [METASOUND_SETUP.md](Docs/METASOUND_SETUP.md) | Audio Engineer | Adaptive audio |
| [FEEDBACK_SETUP.md](Docs/FEEDBACK_SETUP.md) | Tech Artist | Shake, rumble, vignette |
| [VFX_SETUP.md](Docs/VFX_SETUP.md) | VFX Artist | Niagara particles |
| [HUD_SETUP.md](Docs/HUD_SETUP.md) | UI Artist | UMG widget |
| [TUNING_SETUP.md](Docs/TUNING_SETUP.md) | Designer | Difficulty presets |
| [SAVE_LOAD_SETUP.md](Docs/SAVE_LOAD_SETUP.md) | Designer | Persistence |
| [GAMELOOP_SETUP.md](Docs/GAMELOOP_SETUP.md) | Designer | Win/loss flow |
| [OPTIMIZATION_SETUP.md](Docs/OPTIMIZATION_SETUP.md) | Engineer | 780M iGPU perf |
| [DEPLOYMENT_SETUP.md](Docs/DEPLOYMENT_SETUP.md) | Engineer | Packaging |

## Target Hardware
AMD Radeon 780M iGPU — all systems designed for integrated graphics.

## License
Proprietary. All rights reserved.
