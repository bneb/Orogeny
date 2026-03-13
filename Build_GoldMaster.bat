@echo off
REM ============================================================================
REM OROGENY: Gold Master Build Script
REM Day 14 — Shipping Build for Win64
REM ============================================================================
REM
REM USAGE:
REM   1. Open a Command Prompt (not PowerShell)
REM   2. cd to the Orogeny project root
REM   3. Run: Build_GoldMaster.bat
REM
REM PRE-REQUISITES:
REM   - Unreal Engine 5.7 installed
REM   - Visual Studio 2022 with C++ game development workload
REM   - All 69 TDD tests passing (run RunTests.sh first)
REM
REM OUTPUT:
REM   %ARCHIVE_DIR%\WindowsClient\Orogeny.exe
REM ============================================================================

REM --- Configuration ---
SET UE_ROOT=C:\Program Files\Epic Games\UE_5.7
SET PROJECT_PATH=%~dp0Orogeny.uproject
SET TARGET_PLATFORM=Win64
SET TARGET_CONFIG=Shipping
SET ARCHIVE_DIR=%~dp0Build\GoldMaster

REM --- Validate Engine Path ---
IF NOT EXIST "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" (
    echo ERROR: Unreal Engine not found at %UE_ROOT%
    echo        Update UE_ROOT in this script to match your installation.
    exit /b 1
)

REM --- Display Build Info ---
echo ============================================================================
echo  OROGENY: Gold Master Build
echo  Platform:  %TARGET_PLATFORM%
echo  Config:    %TARGET_CONFIG%
echo  Archive:   %ARCHIVE_DIR%
echo ============================================================================
echo.

REM --- Execute BuildCookRun ---
call "%UE_ROOT%\Engine\Build\BatchFiles\RunUAT.bat" ^
    BuildCookRun ^
    -project="%PROJECT_PATH%" ^
    -noP4 ^
    -platform=%TARGET_PLATFORM% ^
    -clientconfig=%TARGET_CONFIG% ^
    -serverconfig=%TARGET_CONFIG% ^
    -build ^
    -cook ^
    -stage ^
    -pak ^
    -compressed ^
    -prereqs ^
    -archive ^
    -archivedirectory="%ARCHIVE_DIR%" ^
    -utf8output ^
    -unattended ^
    -createreleaseversion=1.0.0

REM --- Check Result ---
IF ERRORLEVEL 1 (
    echo.
    echo ============================================================================
    echo  BUILD FAILED — Check the log above for errors.
    echo ============================================================================
    exit /b 1
)

echo.
echo ============================================================================
echo  BUILD SUCCEEDED
echo  Output: %ARCHIVE_DIR%\WindowsClient\
echo ============================================================================
echo.

REM --- Run Verification Script ---
echo Running build verification...
python "%~dp0Tests\Verify_Build.py" "%ARCHIVE_DIR%"
IF ERRORLEVEL 1 (
    echo  VERIFICATION FAILED
    exit /b 1
)

echo.
echo  Gold Master ready for distribution.
echo ============================================================================
