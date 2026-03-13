@echo off
REM ============================================================================
REM Orogeny — Local Build & Test Script
REM ============================================================================
REM Prerequisites:
REM   - Unreal Engine 5.3+ installed
REM   - Set UE5_ROOT below OR as an environment variable
REM ============================================================================

setlocal EnableDelayedExpansion

REM --- Auto-detect UE5 if UE5_ROOT is not set ---
if not defined UE5_ROOT (
    for %%D in (
        "C:\Program Files\Epic Games\UE_5.3"
        "C:\Program Files\Epic Games\UE_5.4"
        "C:\Program Files\Epic Games\UE_5.5"
        "D:\Epic Games\UE_5.3"
        "D:\Epic Games\UE_5.4"
        "D:\Epic Games\UE_5.5"
    ) do (
        if exist "%%~D\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" (
            set "UE5_ROOT=%%~D"
            goto :found
        )
    )
    echo [ERROR] Could not find UE5. Set UE5_ROOT environment variable.
    echo         Example: set UE5_ROOT=C:\Program Files\Epic Games\UE_5.3
    exit /b 1
)
:found

set "UBT=%UE5_ROOT%\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
set "EDITOR_CMD=%UE5_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
set "PROJECT=%~dp0Orogeny.uproject"

echo ============================================================================
echo  OROGENY BUILD ^& TEST
echo  UE5: %UE5_ROOT%
echo  Project: %PROJECT%
echo ============================================================================
echo.

REM --- Step 1: Generate Project Files ---
echo [1/3] Generating project files...
"%UBT%" -projectfiles -project="%PROJECT%" -game -rocket -progress
if %ERRORLEVEL% neq 0 (
    echo [FAIL] Project file generation failed.
    exit /b 1
)
echo [PASS] Project files generated.
echo.

REM --- Step 2: Compile ---
echo [2/3] Compiling OrogenyEditor (Win64 Development)...
"%UBT%" OrogenyEditor Win64 Development -Project="%PROJECT%" -WaitMutex -FromMsBuild
if %ERRORLEVEL% neq 0 (
    echo [FAIL] Compilation failed. Check Saved\Logs for details.
    exit /b 1
)
echo [PASS] Compilation succeeded.
echo.

REM --- Step 3: Run TDD Tests ---
echo [3/3] Running automation tests (headless)...
"%EDITOR_CMD%" "%PROJECT%" -ExecCmds="Automation RunTests Orogeny.Config; Quit" -unattended -nopause -buildmachine -nosplash -nullrhi
if %ERRORLEVEL% neq 0 (
    echo [FAIL] Automation tests failed. Check Saved\Logs for details.
    exit /b 1
)
echo [PASS] All automation tests passed.
echo.

echo ============================================================================
echo  ALL CLEAR — Build and tests passed.
echo ============================================================================
exit /b 0
