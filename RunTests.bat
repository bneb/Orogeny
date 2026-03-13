@echo off
REM ============================================================================
REM Orogeny — Run Tests Only (skip compile)
REM ============================================================================
REM Use this after you've already compiled once. Much faster.
REM ============================================================================

setlocal EnableDelayedExpansion

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
    exit /b 1
)
:found

set "EDITOR_CMD=%UE5_ROOT%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
set "PROJECT=%~dp0Orogeny.uproject"

echo ============================================================================
echo  OROGENY TEST RUNNER
echo  Running: Orogeny.Config.*
echo ============================================================================
echo.

"%EDITOR_CMD%" "%PROJECT%" -ExecCmds="Automation RunTests Orogeny.Config; Quit" -unattended -nopause -buildmachine -nosplash -nullrhi

if %ERRORLEVEL% neq 0 (
    echo.
    echo [FAIL] Tests failed. Check Saved\Logs for details.
    exit /b 1
)

echo.
echo [PASS] All Orogeny.Config tests passed.
exit /b 0
