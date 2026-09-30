#!/bin/sh
# One-command build on Linux/macOS: the committed "linux" CMake preset
# (Unix Makefiles + system compiler). Windows users use build.bat instead.
#
# Usage:  ./build.sh            # incremental
#         ./build.sh clean      # wipe build/ first, e.g. after switching raylib tag
#         ./build.sh -j8        # extra args are forwarded to the build step
set -e

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BUILD="$ROOT/build"
PRESET=linux

for tool in cmake make; do
    command -v "$tool" >/dev/null 2>&1 || {
        echo "[build] $tool not found in PATH"
        exit 1
    }
done
command -v c++ >/dev/null 2>&1 || command -v g++ >/dev/null 2>&1 || {
    echo "[build] no C++ compiler (c++/g++) found in PATH"
    exit 1
}

# Everything after a literal "clean" goes to the compiler.
clean=0
build_args=""
for arg in "$@"; do
    if [ "$arg" = "clean" ] && [ -z "$build_args" ]; then
        clean=1
    else
        # Quoted so the value grows into one string; it is split again on
        # purpose at the cmake --build call below so flags reach the compiler.
        build_args="$build_args $arg"
    fi
done

if [ "$clean" = 1 ]; then
    echo "[build] removing $BUILD"
    rm -rf "$BUILD"
fi

cd "$ROOT"
cmake --preset "$PRESET" || { echo "[build] configure FAILED"; exit 1; }

# Make defaults to a single job, which left a clean build compiling one file at
# a time on an 8-core box. Parallelise unless the caller passed their own -j.
jobs=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
case " $build_args " in
*" -j"* | *" --parallel"*) ;;
*) [ "$jobs" -gt 1 ] 2>/dev/null && build_args="$build_args -j$jobs" ;;
esac

# shellcheck disable=SC2086
cmake --build --preset "$PRESET" $build_args || { echo "[build] compile FAILED"; exit 1; }

if [ ! -x "$BUILD/minecraft" ]; then
    echo "[build] FAILED: build/minecraft not produced"
    exit 1
fi

echo "[build] OK: $BUILD/minecraft ($(wc -c <"$BUILD/minecraft") bytes, built $(date -r "$BUILD/minecraft" '+%Y-%m-%d %H:%M:%S' 2>/dev/null || stat -c %y "$BUILD/minecraft"))"
echo "[build] run it with:  ./run.sh"
