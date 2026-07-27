@echo off
setlocal enableextensions enabledelayedexpansion

echo Setting up FlowCash environment...

:: Create required data directories
if not exist "data\bank_data\accounts" mkdir "data\bank_data\accounts"
if not exist "data\bank_data\transactions" mkdir "data\bank_data\transactions"
if not exist "data\bank_data\loans" mkdir "data\bank_data\loans"

:: Check if GCC compiler is available
where gcc >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo Compiling FlowCash with GCC...
    gcc -Wall -Wextra -std=c11 -Iinclude -o flowcash.exe src\main.c src\admin.c src\user.c src\loan.c src\utils.c src\demo.c
    if !ERRORLEVEL! NEQ 0 (
        echo Compilation failed!
        pause
        exit /b !ERRORLEVEL!
    )
) else if exist "flowcash.exe" (
    echo GCC compiler not found in PATH. Using precompiled flowcash.exe...
) else (
    echo Error: GCC compiler not found and precompiled executable flowcash.exe is missing.
    echo Please install GCC - MinGW-w64 or TDM-GCC - or add it to system PATH.
    pause
    exit /b 1
)

echo Build complete. Running FlowCash...
echo.
flowcash.exe %*

if %ERRORLEVEL% NEQ 0 (
    pause
)
