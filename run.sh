#!/bin/sh
# Launch the game from the project root. Windows users use run.bat instead;
# the working directory is kept at the repo root on purpose so any relative
# path (assets/, saves, …) resolves the same way as on Windows.
#
# Usage:  ./run.sh            # play
#         ./run.sh --flag     # extra args are forwarded to the game
set -e

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BIN="$ROOT/build/minecraft"

if [ ! -x "$BIN" ]; then
    echo "[run] build/minecraft not found - run ./build.sh first"
    exit 1
fi

# raylib bundles GLFW, which dlopen()s libX11.so.6 (plus Xcursor/Xrandr/Xi) at
# runtime instead of linking them, so the X11 client libraries have to be
# visible to the dynamic loader. NixOS keeps them in the system profile, which
# is not on the loader's default search path -- without this the game dies with
# "GLFW: X11: Failed to load Xlib". Inside `nix develop` the profile is already
# wired up, so the check below finds libX11 and leaves LD_LIBRARY_PATH alone.
if ! (ldconfig -p 2>/dev/null || true) | grep -q 'libX11\.so\.6'; then
    for dir in /run/current-system/sw/lib "$HOME/.nix-profile/lib"; do
        if [ -e "$dir/libX11.so.6" ]; then
            LD_LIBRARY_PATH="$dir${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
            export LD_LIBRARY_PATH
            echo "[run] X11 not in the loader path - using LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
            break
        fi
    done
fi

# GLFW centres the new window on the primary monitor and dereferences it
# without a NULL check, so an X server with every output "disconnected" (screen
# present but no display attached) kills the process inside InitWindow(). Only
# checked when xrandr is around; if it is missing we simply fall through.
if [ -n "$DISPLAY" ] && command -v xrandr >/dev/null 2>&1; then
    if ! xrandr --query 2>/dev/null | grep -q " connected"; then
        echo "[run] DISPLAY=$DISPLAY has no connected monitor - GLFW cannot place a window there"
        echo "      power on / plug in a display, or use a virtual one:"
        echo "      Xvfb :99 -screen 0 1920x1080x24 & DISPLAY=:99 $ROOT/run.sh"
        exit 1
    fi
fi

cd "$ROOT"
if "$BIN" "$@"; then
    exit 0
else
    rc=$?
    echo "[run] exited with $rc"
    exit "$rc"
fi
