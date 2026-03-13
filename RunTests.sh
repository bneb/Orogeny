#!/bin/bash
# ============================================================================
# Orogeny — macOS Test Runner
# ============================================================================
# Runs all headless TDD automation tests without opening the Editor GUI.
# No GPU required (-nullrhi). Tests pure C++ math: IK, physics, state logic.
#
# USAGE:
#   chmod +x RunTests.sh
#   ./RunTests.sh
#
# PREREQUISITES:
#   - UE5.3+ installed via Epic Games Launcher
#   - Project compiled at least once (open .uproject first, or use GenerateProjectFiles.sh)
# ============================================================================

set -e

# --- Auto-detect UE5 installation ---
UE_PATHS=(
    "/Users/Shared/Epic Games/UE_5.5/Engine/Binaries/Mac/UnrealEditor-Cmd"
    "/Users/Shared/Epic Games/UE_5.4/Engine/Binaries/Mac/UnrealEditor-Cmd"
    "/Users/Shared/Epic Games/UE_5.3/Engine/Binaries/Mac/UnrealEditor-Cmd"
    "$HOME/Library/Application Support/Epic/UnrealEngine/UE_5.5/Engine/Binaries/Mac/UnrealEditor-Cmd"
    "$HOME/Library/Application Support/Epic/UnrealEngine/UE_5.4/Engine/Binaries/Mac/UnrealEditor-Cmd"
    "$HOME/Library/Application Support/Epic/UnrealEngine/UE_5.3/Engine/Binaries/Mac/UnrealEditor-Cmd"
)

UE_CMD=""

# Check if user set UE5_CMD explicitly
if [ -n "$UE5_CMD" ]; then
    UE_CMD="$UE5_CMD"
else
    for path in "${UE_PATHS[@]}"; do
        if [ -f "$path" ]; then
            UE_CMD="$path"
            break
        fi
    done
fi

if [ -z "$UE_CMD" ]; then
    echo "ERROR: Could not find UnrealEditor-Cmd."
    echo "Set UE5_CMD to the full path, e.g.:"
    echo "  export UE5_CMD=\"/path/to/UnrealEditor-Cmd\""
    exit 1
fi

# --- Locate project file ---
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
UPROJECT="$SCRIPT_DIR/Orogeny.uproject"

if [ ! -f "$UPROJECT" ]; then
    echo "ERROR: Orogeny.uproject not found at $UPROJECT"
    exit 1
fi

echo "============================================"
echo " Orogeny TDD Test Runner (macOS)"
echo "============================================"
echo " Engine:  $UE_CMD"
echo " Project: $UPROJECT"
echo "============================================"
echo ""

# --- Run all Orogeny automation tests headlessly ---
"$UE_CMD" "$UPROJECT" \
    -ExecCmds="Automation RunTests Orogeny; Quit" \
    -unattended \
    -nopause \
    -nosplash \
    -nullrhi \
    -log

EXIT_CODE=$?

echo ""
if [ $EXIT_CODE -eq 0 ]; then
    echo "✅ ALL TESTS PASSED"
else
    echo "❌ TESTS FAILED (exit code: $EXIT_CODE)"
fi

exit $EXIT_CODE
