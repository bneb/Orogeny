@echo off
REM ============================================================================
REM Orogeny — Run Tests Only (skip compile)
REM ============================================================================
REM Works on any Windows machine with UE5.5+ installed.
REM Auto-detects common install paths. Override with: set UE5_ROOT=...
REM ============================================================================

setlocal EnableDelayedExpansion

if not defined UE5_ROOT (
    for %%D in (
        "C:\Program Files\Epic Games\UE_5.7"
        "C:\Program Files\Epic Games\UE_5.6"
        "C:\Program Files\Epic Games\UE_5.5"
        "D:\Epic Games\UE_5.7"
        "D:\Epic Games\UE_5.6"
        "D:\Epic Games\UE_5.5"
        "E:\Epic Games\UE_5.7"
        "E:\Epic Games\UE_5.6"
        "E:\Epic Games\UE_5.5"
    ) do (
        if exist "%%~D\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" (
            set "UE5_ROOT=%%~D"
            goto :found
        )
    )
    echo [ERROR] Could not find UE5. Set UE5_ROOT environment variable.
    echo         Example: set UE5_ROOT=C:\Program Files\Epic Games\UE_5.5
    exit /b 1
)
:found

set "EDITOR_CMD=%UE5_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
set "PROJECT=%~dp0Orogeny.uproject"

echo ============================================================================
echo  OROGENY TEST RUNNER
echo  Engine: %UE5_ROOT%
echo  Running: All Orogeny.* tests (Config, Animation, Movement, State)
echo ============================================================================
echo.

"%EDITOR_CMD%" "%PROJECT%" -ExecCmds="Automation RunTests Orogeny; Quit" -unattended -nopause -buildmachine -nosplash -nullrhi

if %ERRORLEVEL% neq 0 (
    echo.
    echo [FAIL] Tests failed. Check Saved\Logs for details.
    exit /b 1
)

echo.
echo [PASS] All Orogeny tests passed.
exit /b 0
