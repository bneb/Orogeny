# OROGENY: Game Completion Playbook

> **Status:** 139 TDD tests passing | 78 C++ files | All systems code-complete
> **Goal:** Go from compiled code → playable, beautiful game

---

## Phase 1: Foundation (Run Once)

### Step 1: Run the Python Automation Script
This spawns the atmospheric actors, creates the base material, and assigns default meshes.

```
1. Open UE5 Editor → Load your Orogeny project
2. Open: Window → Output Log
3. Type in the command field at the bottom:
   py Scripts/Build_Visuals.py
4. Press Enter
5. Verify Output Log shows:
   - "Spawned DirectionalLight"
   - "Spawned SkyAtmosphere"
   - "Spawned ExponentialHeightFog"
   - "Created M_ProceduralTerrain"
   - "ALL CHECKS PASSED"
```

### Step 2: Configure Project Settings

```
1. Edit → Project Settings → Maps & Modes:
   - Default GameMode = BP_OrogenyGameMode (create from AOrogenyGameModeBase)
   - Default Pawn Class = BP_TitanCharacter (create from ATitanCharacter)
   - Player Controller Class = AOrogenyPlayerController
2. Save All
```

---

## Phase 2: Read Guides & Create Assets (In Order)

> Each step references a specific `.md` file. Read it, follow it, then move to the next.

### Step 3: Deep Time
📖 Read: [DEEPTIME_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/DEEPTIME_SETUP.md)
- Tag your DirectionalLight with `"SunLight"` so UDeepTimeSubsystem can rotate it
- Verify the day/night cycle runs in PIE

### Step 4: Terrain Material
📖 Read: [TRIPLANAR_MATERIAL_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/TRIPLANAR_MATERIAL_SETUP.md)
- Open `M_ProceduralTerrain` (created by Python script)
- Wire the triplanar projection nodes
- Set GrassColor, RockColor, SnowColor, BlightedColor parameters
- Assign to your terrain chunks

### Step 5: Atmosphere
📖 Read: [ATMOSPHERE_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/ATMOSPHERE_SETUP.md) then [ATMOSPHERE_DIRECTOR_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/ATMOSPHERE_DIRECTOR_SETUP.md)
- Place `AAtmosphericDirector` in the level
- It auto-finds SkyAtmosphere and ExponentialHeightFog
- Verify fog/sky shift with day/night cycle

### Step 6: Ecosystem & Flora
📖 Read: [ECOSYSTEM_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/ECOSYSTEM_SETUP.md)
- Assign forest/grass meshes to the HISM slots on `UEcosystemArmorComponent`
- Verify flora spawns on the Titan

### Step 7: Blight
📖 Read: [BLIGHT_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/BLIGHT_SETUP.md)
- Assign a Blight mesh to `AIronBlightSeed`
- Place seed actors in the level or let the system spawn them
- Verify cellular automata corruption spreads

### Step 8: Subduction
📖 Read: [SUBDUCTION_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/SUBDUCTION_SETUP.md)
- Verify `UTitanSubductionComponent` is on the Titan BP
- Test subduction input sinks the Titan into the ground

### Step 9: Combat & Supercell
📖 Read: [COMBAT_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/COMBAT_SETUP.md) then [SUPERCELL_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/SUPERCELL_SETUP.md)
- Place `ASupercell_Actor` in the level
- Configure storm radius and speed
- Verify Lift/Weaken combat mechanics

### Step 10: Audio
📖 Read: [METASOUND_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/METASOUND_SETUP.md) then [AUDIO_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/AUDIO_SETUP.md)
- Create `MS_OrogenyAmbience` MetaSoundSource
- Add float inputs: `WindIntensity`, `FaunaVolume`, `TimePitch`
- Design the sound graph (wind, fauna chorus, time drone)
- Assign to `AMetaSoundDirector` in the level

### Step 11: Game Feel (Camera Shake, Rumble, Vignette)
📖 Read: [FEEDBACK_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/FEEDBACK_SETUP.md)
- Create `CS_FootstepHeave` Camera Shake BP (Perlin, low frequency, heavy amplitude)
- Create `CS_SubductionGrind` Camera Shake BP
- Create `FF_SubductionRumble` Force Feedback asset
- Assign all three to `UTectonicFeedbackComponent` on the Titan
- Place a Post Process Volume → enable Vignette, set intensity to 0 (code drives it)

### Step 12: VFX (Particles)
📖 Read: [VFX_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/VFX_SETUP.md)
- Create `NS_FootstepDust` (looping, User Param: `SpawnRate`)
- Create `NS_SubductionDebris` (burst, User Param: `BurstCount`)
- Create `NS_BlightSmoke` (looping, User Param: `SmokeOpacity`)
- Assign to `UTitanVFXComponent` on the Titan

### Step 13: HUD
📖 Read: [HUD_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/HUD_SETUP.md)
- Create `WBP_OrogenyHUD` Widget Blueprint (parent: `UOrogenyHUDWidget`)
- Add Text Blocks: bind to `GetDisplayCentury`, `GetDisplayHealth`
- Add Progress Bar: bind to `GetDisplayBlightThreat`
- Aesthetic: Garamond/Trajan font, minimal, no boxes
- Assign `WBP_OrogenyHUD` to `HUDWidgetClass` on `AOrogenyPlayerController`

### Step 14: Difficulty Presets
📖 Read: [TUNING_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/TUNING_SETUP.md)
- Create 3 Data Assets (from `UOrogenyDifficultyPreset`):
  - `DA_Diff_Story`: Blight ×0.5, Heal ×2.0, 5 centuries
  - `DA_Diff_Survival`: Blight ×1.0, Heal ×1.0, 10 centuries
  - `DA_Diff_Endless`: Blight ×1.5, Heal ×1.0, 0 centuries (endless)
- Assign one to `ActiveDifficulty` on the GameMode

### Step 15: Save/Load
📖 Read: [SAVE_LOAD_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/SAVE_LOAD_SETUP.md)
- Wire Save/Load buttons in your Pause Menu to `USaveLoadSubsystem`
- Test: play → move → save → restart → load → verify position restored

---

## Phase 3: Polish & Verify

### Step 16: Map & Level Assembly
📖 Read: [MAP_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/MAP_SETUP.md)
- Ensure all actors are placed and configured
- Verify chunk streaming around the Titan

### Step 17: Game Loop Verification
📖 Read: [GAMELOOP_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/GAMELOOP_SETUP.md)
- Play through a full cycle: survive → victory, or die → defeat
- Test all three difficulty presets

### Step 18: Animation
📖 Read: [ANIM_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/ANIM_SETUP.md)
- Set up Titan animation blueprint (idle, walk, subduction)

### Step 19: Vertical Slice
📖 Read: [VERTICAL_SLICE_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/VERTICAL_SLICE_SETUP.md)
- Full playtest from start to finish
- Capture footage for trailers

### Step 20: Aesthetics
📖 Read: [AESTHETIC_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/AESTHETIC_SETUP.md)
- Post-process color grading, bloom, film grain
- Romantic landscape art direction pass

### Step 21: Optimization
📖 Read: [OPTIMIZATION_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/OPTIMIZATION_SETUP.md)
- Profile on target hardware (AMD 780M iGPU)
- `stat fps`, `stat unit`, `stat niagara`
- LOD tuning, draw call reduction

### Step 22: Deployment
📖 Read: [DEPLOYMENT_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/DEPLOYMENT_SETUP.md)
- Package for Windows/Mac
- Steam build configuration

### Step 23: Trench (Advanced)
📖 Read: [TRENCH_SETUP.md](file:///Users/kevin/projects/orogeny/Docs/TRENCH_SETUP.md)
- Advanced terrain features if needed

---

## Quick Reference: Asset Checklist

| Asset | Type | Assign To |
|-------|------|-----------|
| `M_ProceduralTerrain` | Material | Terrain Chunks |
| `MS_OrogenyAmbience` | MetaSoundSource | AMetaSoundDirector |
| `CS_FootstepHeave` | Camera Shake | UTectonicFeedbackComponent |
| `CS_SubductionGrind` | Camera Shake | UTectonicFeedbackComponent |
| `FF_SubductionRumble` | Force Feedback | UTectonicFeedbackComponent |
| `NS_FootstepDust` | Niagara System | UTitanVFXComponent |
| `NS_SubductionDebris` | Niagara System | UTitanVFXComponent |
| `NS_BlightSmoke` | Niagara System | UTitanVFXComponent |
| `WBP_OrogenyHUD` | Widget Blueprint | AOrogenyPlayerController |
| `DA_Diff_Story` | Data Asset | AOrogenyGameModeBase |
| `DA_Diff_Survival` | Data Asset | AOrogenyGameModeBase |
| `DA_Diff_Endless` | Data Asset | AOrogenyGameModeBase |

---

## Quick Reference: C++ Test Count

```
139 tests across 27 suites — ALL PASSING
Run: ./RunTests.sh
```

> **The code is complete. Follow this playbook top to bottom.
> By Step 19 you will have a playable vertical slice.**
