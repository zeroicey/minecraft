# Minecraft Clone

A simple Minecraft-like voxel game built with C++ and Raylib, featuring procedural terrain generation, chunk-based world management, and basic player controls.
<img width="2546" height="1440" alt="image" src="https://github.com/user-attachments/assets/8ac4baf6-dd86-436d-b379-633bbdb9c6a7" />

## Features

- **Procedural Terrain Generation**: Infinite world generation with varied terrain using noise-based algorithms
- **Chunk-based World Management**: Efficient memory management with a 16x256x16 chunk system
- **First-Person Camera**: Smooth mouse-look controls and WASD movement
- **Block System**: Multiple block types (grass, dirt, stone) with textures
- **Dynamic Chunk Loading**: Automatically loads/unloads chunks based on player position
- **Optimized Rendering**: 3D voxel rendering with Raylib

## Prerequisites

- **CMake** (version 3.15 or higher)
- **C++ Compiler** with C++17 support (GCC, Clang, or MSVC)

Dependencies (raylib, including its bundled GLFW) are downloaded by CMake itself
over HTTPS and checked against a pinned SHA-256, so Git is not required.

The download is cached in `.cache/raylib-<version>.tar.gz` (gitignored). If your
link is slow or flaky, fetching that file once by hand — with a resumable
`curl -C -`, for example — lets every later `build.sh clean` reuse it instead of
re-pulling ~42 MB.

## Building the Project

One-command scripts, one per platform (they run the committed CMake presets):

| Platform | Build | Run | Preset |
| --- | --- | --- | --- |
| Windows (MinGW-w64 + Ninja) | `build.bat` | `run.bat` | `windows-mingw` |
| Linux / macOS (system compiler + Make) | `./build.sh` | `./run.sh` | `linux` |

```bash
./build.sh          # incremental
./build.sh clean    # wipe build/ first, e.g. after switching the raylib tag
./build.sh -j8      # extra args go to the build step
./run.sh            # launch the game
```

If you prefer to drive CMake yourself:

1. Clone the repository:
```bash
git clone https://github.com/zeroicey/minecraft.git
cd minecraft
```

2. Configure with CMake:
```bash
cmake --preset linux        # or: cmake -B build
```

3. Build the project:
```bash
cmake --build --preset linux
```

## Running the Game

After building, launch it from the project root so relative paths resolve:

```bash
./run.sh            # or: ./build/minecraft
```

The game will launch in a 1920x1080 window at 120 FPS.

On NixOS the X11 client libraries live in the system profile, which is not on
the dynamic loader's default search path, and GLFW `dlopen()`s them by name --
running the binary directly would die with
`GLFW: X11: Failed to load Xlib`. `./run.sh` detects that and puts
`/run/current-system/sw/lib` on `LD_LIBRARY_PATH` for you. Without a reachable
display the game prints an error and exits 1 instead of crashing.

If `DISPLAY` is set but every output is reported as `disconnected` (the X server
is up, no physical display attached), GLFW cannot centre a window and the game
would abort inside `InitWindow()`; `./run.sh` detects this and tells you. Either
attach a display or run against a virtual one:

```bash
Xvfb :99 -screen 0 1920x1080x24 & DISPLAY=:99 ./run.sh
```

## Controls

- **W/A/S/D** - Move forward/left/backward/right
- **Mouse** - Look around
- **ESC** - Exit game

## Project Structure

```
minecraft/
├── build.bat              # Windows build (MinGW-w64 + Ninja preset)
├── run.bat                # Windows launcher
├── build.sh               # Linux/macOS build (system compiler + Make preset)
├── run.sh                 # Linux/macOS launcher
├── CMakeLists.txt          # Build configuration
├── CMakePresets.json      # Shared presets used by the four scripts above
├── assets/                 # Source art
│   └── atlas.png          # Texture atlas (16x16 tiles), compiled into the exe
├── cmake/                  # Build helpers
│   └── embed_binary.cmake # Turns a binary file into an embedded byte array
├── include/               # Header files
│   ├── block.h           # Block type definitions
│   ├── chunk.h           # Chunk management
│   ├── config.h          # Game configuration
│   ├── player.h          # Player/camera controls
│   ├── texture_atlas.h   # Texture atlas + UV lookup
│   ├── utils.h           # Utility functions
│   └── world.h           # World management
└── src/                  # Source files
    ├── main.cpp          # Entry point
    ├── block.cpp         # Block implementation
    ├── chunk.cpp         # Chunk implementation
    ├── player.cpp        # Player controls
    ├── ui.cpp            # UI rendering
    ├── utils.cpp         # Utility implementations
    └── world.cpp         # World generation and management
```

## Configuration

Game settings can be modified in `include/config.h`:

- **Screen Resolution**: `SCREEN_WIDTH`, `SCREEN_HEIGHT` (default: 1920x1080)
- **Frame Rate**: `TARGET_FPS` (default: 120)
- **Player Movement**: `PLAYER_MOVE_SPEED` (default: 0.1)
- **Mouse Sensitivity**: `PLAYER_MOUSE_SENSITIVITY` (default: 0.030)
- **Terrain Generation**:
  - `TERRAIN_BASE_HEIGHT`: Base terrain height (default: 32.0)
  - `TERRAIN_AMPLITUDE`: Terrain variation (default: 15.0)
  - `TERRAIN_FREQUENCY`: Terrain smoothness (default: 10.0)
  - `STONE_LAYER_DEPTH`: Depth where stone begins (default: 5)

## Technologies Used

- **[Raylib](https://www.raylib.com/)**: Graphics and game engine framework
- **C++17**: Programming language
- **CMake**: Build system
- **STL**: Standard Template Library for data structures (maps, etc.)

## Technical Details

### Chunk System
- Chunks are 16x256x16 blocks
- Dynamically loaded/unloaded based on player position
- Efficient memory management using `std::map`

### World Generation
- Procedural terrain generation using noise algorithms
- Layered block generation (grass → dirt → stone)
- Configurable terrain parameters

### Block Registry
- Centralized block type management
- Supports multiple block types with different properties
- Texture management for each block type

## License

This project is for educational purposes.
