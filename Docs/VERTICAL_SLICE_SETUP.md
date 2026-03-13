# Vertical Slice: Game Loop Setup Guide

> **Audience:** Tech Artist / UI Designer
> **System:** AOrogenyGameModeBase Sprint 5 Update

## Architecture

```
AOrogenyGameModeBase::Tick
  ├─ Day 12: Storm exposure → EvaluateGameState (still active)
  ├─ Sprint 5: Deep Time centuries → EvaluateDeepTimeGameState
  │    ├─ Read UDeepTimeSubsystem::CurrentDay / 36525 → centuries
  │    └─ Read UEcosystemArmorComponent::EcosystemHealth
  └─ Deep Time state overrides Day 12 if Day 12 = Playing
```

## Win / Loss Conditions

| Condition | Trigger | Priority |
|-----------|---------|----------|
| **Victory** | Survive 10 centuries (1,000 years Deep Time) | 2nd |
| **Defeat (Storm)** | Storm exposure ≥ 10 seconds | 1st |
| **Defeat (Blight)** | Ecosystem health ≤ 0 (bare mountain) | 1st |

**Loss always overrides win.** If the Mountain reaches 10 centuries but has 0 health, the Blight wins.

## UI Widget Updates

Replace the 15-minute countdown with Deep Time display:

### Reading Century Count
```cpp
// In your UI Widget Blueprint:
UDeepTimeSubsystem* DeepTime = GetWorld()->GetSubsystem<UDeepTimeSubsystem>();
float Centuries = DeepTime->CurrentDay / 36525.0f;
int32 Year = FMath::FloorToInt(DeepTime->CurrentDay / 365.25f);
```

### Suggested Display Format
```
ERA: Year 847 of the Third Century
ECOSYSTEM: ████████░░ 80%
```

### Ecosystem Health Bar
```cpp
UEcosystemArmorComponent* Eco = PlayerPawn->FindComponentByClass<UEcosystemArmorComponent>();
float Health = Eco->EcosystemHealth; // [0, 1]
// Map to progress bar width
```
