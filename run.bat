@echo off
REM Launch the game from the project root: textures are loaded with the relative
REM path "assets/grass.png" (see src/world.cpp), so the working directory matters.
setlocal

set "ROOT=%~dp0"
if exist "C:\Program Files\mingw64\bin" set "PATH=C:\Program Files\mingw64\bin;%PATH%"

if not exist "%ROOT%build\minecraft.exe" (
    echo [run] build\minecraft.exe not found - run build.bat first
    exit /b 1
)

pushd "%ROOT%"
"%ROOT%build\minecraft.exe" %*
set "RC=%ERRORLEVEL%"
popd
if not "%RC%"=="0" echo [run] exited with %RC%
endlocal
