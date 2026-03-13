# Save/Load: Setup Guide

> **Audience:** UI Designer / Game Designer
> **System:** USaveLoadSubsystem (Sprint 14)

## What It Does

The `USaveLoadSubsystem` persists the entire game state to disk and restores it on load. **All serialization is async — zero frame hitches.** Loading instantly updates Deep Time, Ecosystem Health, Titan position, and the Blight grid.

## Saved State

| Property | Source | Type |
|----------|--------|------|
| Deep Time Day | `UDeepTimeSubsystem` | double |
| Ecosystem Health | `UEcosystemArmorComponent` | float |
| Titan Location | `ATitanCharacter` | FVector |
| Blight Grid | Cellular automata (corrupted cells only) | TArray |

## Step 1: Hook Up Pause Menu

### Save Button
1. In your Pause Menu Widget Blueprint, add a **Save Button**.
2. On Click → **Get Game Instance** → **Get Subsystem** → `SaveLoadSubsystem`.
3. Call **Save Game** with slot name `"Slot1"`.

### Load Button
1. Add a **Load Button**.
2. On Click → same subsystem → Call **Load Game** with `"Slot1"`.

### Blueprint Graph
```
[Save Button Click]
  → Get Game Instance
  → Get Subsystem (SaveLoadSubsystem)
  → Save Game ("Slot1")
```

## Step 2: Verify

1. Play in Editor → move the Titan, let time pass.
2. Open Pause Menu → click Save.
3. Output Log shows: `SaveLoad: ✅ Saved to slot 'Slot1'`
4. Stop PIE → Play again → click Load.
5. Titan teleports to saved position, time and health restored.
6. Procedural terrain auto-regenerates around the loaded coordinates on the next chunk tick.

## Step 3: Multiple Slots (Optional)

Pass different slot names: `"Slot1"`, `"Slot2"`, `"Slot3"`.
Each slot is an independent save file on disk.

## Performance Notes

- **Async I/O** — save/load never blocks the game thread
- **Compressed Blight** — only corrupted cells serialized (not the entire grid)
- Save files are small (~1 KB for typical sessions)
