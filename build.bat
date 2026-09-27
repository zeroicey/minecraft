@echo off
REM One-command build on Windows: MinGW-w64 gcc + Ninja via the committed preset.
REM Usage:  build.bat            (incremental)
REM         build.bat clean      (wipe build/ first, e.g. after switching raylib tag)
setlocal

set "ROOT=%~dp0"
set "BUILD=%ROOT%build"

REM CMake, Ninja, gcc/g++ and the MinGW runtime DLLs all live in this one directory.
if exist "C:\Program Files\mingw64\bin" set "PATH=C:\Program Files\mingw64\bin;%PATH%"

where cmake >nul 2>&1 || (echo [build] cmake not found in PATH & exit /b 1)
where ninja  >nul 2>&1 || (echo [build] ninja not found in PATH  & exit /b 1)
where g++    >nul 2>&1 || (echo [build] g++ not found in PATH    & exit /b 1)

if /i "%~1"=="clean" (
    echo [build] removing %BUILD%
    rmdir /s /q "%BUILD%" 2>nul
)

pushd "%ROOT%"
cmake --preset windows-mingw || (popd & echo [build] configure FAILED & exit /b 1)
cmake --build --preset windows-mingw || (popd & echo [build] compile FAILED & exit /b 1)
popd

if not exist "%BUILD%\minecraft.exe" (
    echo [build] FAILED: build\minecraft.exe not produced
    exit /b 1
)
for %%F in ("%BUILD%\minecraft.exe") do echo [build] OK: %%~fF  (%%~zF bytes, built %%~tF)
echo [build] run it with:  run.bat
endlocal
