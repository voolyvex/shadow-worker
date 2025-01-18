# Shadow Worker Prototype

A 2D game prototype built with Godot 4 and GDExtension (C++), with potential for 3D expansion.

## Requirements

- Godot 4.3 (stable)
- C++ compiler (Visual Studio 2019 or later for Windows)
- SCons build system
- Godot C++ bindings (godot-cpp)

## Setup

1. Clone the repository:
```bash
git clone https://github.com/yourusername/shadow-worker.git
cd shadow-worker
```

2. Make sure you have the Godot C++ bindings at `C:/Godot/godot-cpp`
```bash
cd C:/Godot/godot-cpp
git checkout 4.3-stable
scons platform=windows target=template_debug
scons platform=windows target=template_release
```
3. Build the native library:
```bash
cd /path/to/shadow-worker
scons platform=windows target=template_debug
```

## Controls

- Movement: WASD or Arrow keys
- Interact: E key

## Features

- Smooth character movement with acceleration/deceleration
- NPC interaction system with dialogue
- Basic tilemap-based world
- Collision detection
- Camera following

## Project Structure

- `src/` - C++ source files
  - `player.h/cpp` - Player character implementation
  - `npc.h/cpp` - NPC system implementation
  - `register_types.h/cpp` - GDExtension registration
- `scenes/` - Godot scene files
  - `Main.tscn` - Main game scene
- `resources/` - Game resources
  - `tileset.tres` - Tilemap tileset
  - `npc_dialogue.tres` - NPC dialogue data
- `assets/` - Game assets
  - `tiles/` - Tilemap textures
  - `player.png` - Player sprite
  - `npc.png` - NPC sprite
- `scripts/` - GDExtension native script resources
  - `player.gdns` - Player script resource
  - `npc.gdns` - NPC script resource

## Development

1. Build the native library whenever C++ code changes:
```bash
scons platform=windows target=template_debug
```

2. Open the project in Godot Editor
3. Run the game (F5)

## Performance Targets

- Frame rate: 60+ FPS
- Memory usage: <100MB
- Load time: <2 seconds
- Input latency: <16ms

## License

MIT License - See LICENSE file for details 