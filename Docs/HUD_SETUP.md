# HUD: Setup Guide

> **Audience:** UI Artist / Designer
> **System:** UOrogenyHUDWidget + AOrogenyPlayerController (Sprint 12)

## What It Does

The C++ backend `UOrogenyHUDWidget` reads live game state and exposes **three display-ready getters**. You design the visuals — the code handles all the data plumbing.

## Step 1: Create the Widget Blueprint

1. **Content Browser** → Right-click → **User Interface** → **Widget Blueprint**.
2. In the "Pick Parent Class" dialog, select **OrogenyHUDWidget** (the C++ class).
3. Name it: `WBP_OrogenyHUD`.

## Step 2: Design the Overlay

### Aesthetic Direction
- **Romantic minimalism** — Garamond, Trajan, or EB Garamond (Google Fonts)
- **No boxes or panels** — text floats directly on the viewport
- **Muted opacity** — the world is the star, not the UI
- **Bottom-left anchor** for era info, bottom-right for health

### Suggested Layout
```
┌──────────────────────────────────────────────┐
│                                              │
│                   [GAME WORLD]               │
│                                              │
│                                              │
│  Century VII                   Health: 84%   │
│  ░░░░░░░░░░░ Blight: 12%                    │
└──────────────────────────────────────────────┘
```

## Step 3: Bind Data to Widgets

### Century Text
1. Add a **Text Block** → Name: `CenturyText`.
2. Bind → Create Binding → In the graph:
   - Call **Get Display Century** (from the C++ parent class).
   - Format: `FString::Printf("Century %d", Value)`.

### Health Text
1. Add a **Text Block** → Name: `HealthText`.
2. Bind → Create Binding → Call **Get Display Health**.
3. Format: `FString::Printf("Health: %d%%", Value)`.

### Blight Threat Bar
1. Add a **Progress Bar** → Name: `BlightBar`.
2. Bind Percent → Create Binding → Call **Get Display Blight Threat**.
3. Set bar color to lerp from green → amber → red based on value.

## Step 4: Assign to Player Controller

1. Open **Project Settings** → **Maps & Modes**.
2. Set **Player Controller Class** = `AOrogenyPlayerController`.
3. Open the PlayerController BP (or create `BP_OrogenyPC` from `AOrogenyPlayerController`).
4. Set **HUDWidgetClass** = `WBP_OrogenyHUD`.

## Step 5: Verify in PIE

1. Play in Editor → HUD should appear automatically.
2. Century advances as Deep Time ticks.
3. Health reflects the Ecosystem state.

## Performance Notes

- **No Tick** — getters are polled lazily by UMG binding
- Zero Blueprint tick cost
- All math in pure C++ (FUIMath, TDD-proven)
