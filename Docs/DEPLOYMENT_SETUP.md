# Deployment: Gold Master Build Guide

> **Audience:** Build Engineer / Developer
> **System:** Build_GoldMaster.bat + Verify_Build.py (Day 14)

## Architecture

```
Build_GoldMaster.bat
  └─ RunUAT.bat BuildCookRun
       ├─ -platform=Win64
       ├─ -clientconfig=Shipping
       ├─ -build -cook -stage -pak -compressed
       ├─ -archive → Build/GoldMaster/
       └─ Verify_Build.py (auto-runs after)
             ├─ Check: Orogeny.exe exists
             └─ Check: .pak > 50MB (assets cooked)
```

## Step 1: Pre-Flight Checklist

- [ ] All 69 TDD tests passing (`RunTests.sh`)
- [ ] `MAP_Orogeny_Testbed` exists in Content/Maps/
- [ ] Visual Studio 2022 installed with C++ game dev workload
- [ ] UE 5.7 installed (update `UE_ROOT` in script if different path)

## Step 2: Execute Build

```batch
cd C:\path\to\orogeny
Build_GoldMaster.bat
```

**Expected duration:** 5-15 minutes depending on hardware.

## Step 3: Locate Output

```
orogeny/
  Build/
    GoldMaster/
      WindowsClient/
        Orogeny.exe              ← The game
        Orogeny/
          Content/
            Paks/
              Orogeny-Windows.pak  ← Cooked assets
```

## Step 4: Distribution

### Steam (SteamPipe)
1. Create a SteamPipe depot config pointing to `Build/GoldMaster/WindowsClient/`
2. Set the launch option to `Orogeny.exe`
3. Upload via `steamcmd +app_build`

### Epic Games Store
1. Use BuildPatch Tool (BPT)
2. Point to `Build/GoldMaster/WindowsClient/`
3. Set executable to `Orogeny.exe`

### Standalone / Itch.io
1. Zip the entire `WindowsClient/` folder
2. Upload as-is — no runtime dependencies needed (prereqs included)

## Configuration Reference

### DefaultEngine.ini
```ini
[/Script/EngineSettings.GameMapsSettings]
GameDefaultMap=/Game/Maps/MAP_Orogeny_Testbed.MAP_Orogeny_Testbed
GlobalDefaultGameMode=/Script/Orogeny.OrogenyGameModeBase
GlobalDefaultServerGameMode=/Script/Orogeny.OrogenyGameModeBase
```

### DefaultGame.ini
```ini
[/Script/UnrealEd.ProjectPackagingSettings]
BuildConfiguration=PPBC_Shipping
bCookAll=True
bCompressed=True
bShareMaterialShaderCode=True
bNativizeBlueprintAssets=False
```
