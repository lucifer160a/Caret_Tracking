@echo off
REM Build script for Windows Caret Tracker application
REM Usage: build.bat [Debug|Release] [Win32|x64]

setlocal enabledelayedexpansion

set CONFIG=%1
set PLATFORM=%2

if "%CONFIG%"=="" set CONFIG=Release
if "%PLATFORM%"=="" set PLATFORM=x64

echo.
echo ========================================
echo Caret Tracker - Windows Build
echo ========================================
echo Configuration: %CONFIG%
echo Platform: %PLATFORM%
echo.

REM Check if MSBuild is available
where msbuild >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: MSBuild not found. Please install Visual Studio 2019 or later.
    exit /b 1
)

REM Build the project
echo Building Project2.vcxproj...
msbuild Project2.vcxproj /p:Configuration=%CONFIG% /p:Platform=%PLATFORM% /m

if %errorlevel% neq 0 (
    echo.
    echo Build failed!
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.

REM Determine output directory
set OUTDIR=bin\%PLATFORM%\%CONFIG%

if exist "%OUTDIR%\Project2.exe" (
    echo Output: %OUTDIR%\Project2.exe
    echo.
    echo To run the application:
    echo   %OUTDIR%\Project2.exe
) else (
    echo Warning: Output executable not found at expected location
)

endlocal
