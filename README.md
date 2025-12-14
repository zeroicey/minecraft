# Minecraft Clone

A simple Minecraft-like voxel game built with C++ and Raylib, featuring procedural terrain generation, chunk-based world management, and basic player controls.

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
- **Git** (for fetching dependencies)

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/zeroicey/minecraft.git
cd minecraft
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure with CMake:
```bash
cmake ..
```

4. Build the project:
```bash
cmake --build .
```

## Running the Game

After building, run the executable from the build directory:

```bash
./minecraft
```

The game will launch in a 1920x1080 window at 120 FPS.

## Controls

- **W/A/S/D** - Move forward/left/backward/right
- **Mouse** - Look around
- **ESC** - Exit game

## Project Structure

```
minecraft/
├── CMakeLists.txt          # Build configuration
├── assets/                 # Game assets
│   ├── grass.png          # Grass block texture
│   ├── dirt.png           # Dirt block texture
│   └── stone.png          # Stone block texture
├── include/               # Header files
│   ├── block.h           # Block type definitions
│   ├── chunk.h           # Chunk management
│   ├── config.h          # Game configuration
│   ├── player.h          # Player/camera controls
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
