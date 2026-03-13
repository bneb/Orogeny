# iGPU Optimization: Scalability Pipeline Setup Guide

> **Audience:** Tech Artist / Performance Engineer
> **System:** UOrogenyGameInstance (Day 13)
> **Target:** AMD Radeon 780M iGPU — 60 FPS (16.6ms frame budget)

## Architecture

```
UOrogenyGameInstance
  └─ Init() → ApplyOptimizedScalability()
       ├─ sg.ResolutionQuality = 70     (TSR upscales to native)
       ├─ sg.PostProcessQuality = 2     (High — Romantic aesthetic)
       ├─ sg.ShadowQuality = 2          (High VSM)
       ├─ sg.GlobalIlluminationQuality = 2 (Software Lumen)
       ├─ sg.FoliageQuality = 2
       ├─ sg.EffectsQuality = 2
       ├─ r.TextureStreaming = 1         (enabled)
       ├─ r.Streaming.PoolSize = 3072   (3GB cap for shared RAM)
       └─ r.Shadow.MaxResolution = 1024 (VRAM saver)
```

**Enforcement:** `IConsoleManager::FindConsoleVariable()->Set(ECVF_SetByCode)`

## Step 1: Verify GameInstance Override

1. Edit → **Project Settings** → **Maps & Modes**.
2. Under **Game Instance**, verify it shows `OrogenyGameInstance`.
3. If not, check `Config/DefaultEngine.ini` has:
   ```
   [/Script/Engine.Engine]
   GameInstanceClass=/Script/Orogeny.OrogenyGameInstance
   ```

## Step 2: Verify Performance In-Editor

1. Play in Editor (PIE).
2. Open console (`~` key) → type `stat unit`.
3. Verify:
   - **Game Thread** < 16.6ms
   - **GPU** < 16.6ms
   - **Draw** < 16.6ms

## Step 3: Verify CVar Enforcement

In console (`~` key), type each CVar name to verify the value:
```
sg.ResolutionQuality           → 70
sg.GlobalIlluminationQuality   → 2
r.Streaming.PoolSize           → 3072
r.Shadow.MaxResolution         → 1024
```

## GC Tuning (DefaultEngine.ini)

```ini
[/Script/Engine.GarbageCollectionSettings]
TimeBetweenPurgingPendingKillObjects=61.0   ; Purge once per minute
SizeOfPermanentObjectPool=104857600         ; 100MB permanent pool
```

This prevents micro-stutters from UE5's default aggressive garbage collection.

## Why These Values?

| CVar | Value | Rationale |
|------|-------|-----------|
| ResolutionQuality | 70 | TSR upscales 70% → native, massive fill-rate savings |
| PostProcessQuality | 2 | High preserves our Romantic aesthetic without Epic overhead |
| ShadowQuality | 2 | High VSM, no cinematic cascades eating VRAM |
| GI Quality | 2 | Software Lumen High — no hardware RT needed |
| Streaming.PoolSize | 3072 | 3GB cap — 780M uses system RAM |
| Shadow.MaxResolution | 1024 | Crucial VRAM saver for shared memory |
