# Animation Blueprint Setup Guide — Foot Placement IK

This guide explains how to connect the C++ `UTitanAnimInstance` IK solver to an Animation Blueprint in UE5.

## Prerequisites

- Project compiles with Day 2 changes (UTitanAnimInstance, FootPlacementIKTest)
- A humanoid Skeletal Mesh imported with bones named `foot_l` and `foot_r`
- Day 1 `MAP_Orogeny_Testbed` level created (see `Docs/MAP_SETUP.md`)

## Step 1: Create the Animation Blueprint

1. **Content Browser → Right-click → Animation → Animation Blueprint**
2. **Parent Class**: Select `TitanAnimInstance` (the C++ class)
3. **Skeleton**: Select your imported humanoid skeleton
4. **Name**: `ABP_Titan`
5. **Save to**: `Content/Characters/Titan/`

## Step 2: AnimGraph — Base Pose

1. Open `ABP_Titan` → **AnimGraph** tab
2. Add a **Locomotion State Machine** node:
   - **Idle** state: Idle animation (looping)
   - **Walk** state: Walk cycle (looping, root motion enabled)
   - Transition Idle → Walk: `Speed > 10.0`
   - Transition Walk → Idle: `Speed < 5.0`
3. Connect the State Machine output to the **Output Pose**

## Step 3: Two-Bone IK Nodes (Foot Placement)

After the State Machine, add IK nodes in sequence:

### Left Foot IK
1. Add **Two Bone IK** node after the State Machine
2. Settings:
   - **IK Bone**: `foot_l`
   - **Effector Location Space**: `Bone Space`
   - **Effector Target**: `foot_l`
3. **Effector Location**: Promote to variable, or:
   - Right-click → **Get Left Foot IK Offset** (from your AnimInstance)
   - This reads `LeftFootIKOffset` computed by `CalculateFootIKFromTraceResults()`

### Right Foot IK
1. Add another **Two Bone IK** node after the left one
2. Settings:
   - **IK Bone**: `foot_r`
   - **Effector Location Space**: `Bone Space`
   - **Effector Target**: `foot_r`
3. **Effector Location**: Read from `RightFootIKOffset`

### Hip Adjustment
1. Add a **Modify Bone** node before the IK nodes
2. Settings:
   - **Bone to Modify**: `pelvis` (or `hips`, depends on skeleton)
   - **Translation Mode**: `Add to Existing`
   - **Translation**: `(0, 0, HipOffset)` — read from AnimInstance

### Foot Rotation
1. After each Two Bone IK, add a **Modify Bone** for the foot
2. Settings:
   - **Bone**: `foot_l` / `foot_r`
   - **Rotation Mode**: `Replace Existing`
   - **Rotation**: Read from `LeftFootRotation` / `RightFootRotation`

## Step 4: Verify Bone Names

If your skeleton uses different names, update in C++:

```cpp
// In TitanCharacter.cpp
const FName ATitanCharacter::LeftFootBoneName = FName(TEXT("your_left_foot_name"));
const FName ATitanCharacter::RightFootBoneName = FName(TEXT("your_right_foot_name"));
```

And in the AnimInstance defaults:
```cpp
// In TitanAnimInstance constructor or Blueprint defaults
LeftFootBoneName = FName(TEXT("your_left_foot_name"));
RightFootBoneName = FName(TEXT("your_right_foot_name"));
```

## Step 5: Assign to Character

1. Open your Titan character Blueprint (or `ATitanCharacter` defaults)
2. Select the **Skeletal Mesh Component**
3. Set **Anim Class** to `ABP_Titan`

## Step 6: Test

1. Place the character on uneven terrain in `MAP_Orogeny_Testbed`
2. Enter PIE — observe:
   - Feet align to surface normals on slopes
   - Hips lower to accommodate elevation differences
   - No foot clipping through geometry
3. Console validation:
   ```
   stat anim
   ```
   - IK evaluation should be **< 0.5ms**

## Headless Test Verification

```
UnrealEditor-Cmd.exe "path\to\Orogeny.uproject" -ExecCmds="Automation RunTests Orogeny.Animation; Quit" -unattended -nopause -buildmachine -nosplash -nullrhi
```
