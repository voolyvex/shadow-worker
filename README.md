# Shadow Worker

A 2D game prototype built with raylib, focusing on psychological horror and surreal environments.

## Requirements

- C compiler (Visual Studio 2019 or later for Windows)
- CMake 3.14 or later
- raylib 4.5.0

## Setup

1. Clone the repository:
```bash
git clone https://github.com/yourusername/shadow-worker.git
cd shadow-worker
```

2. Build the project:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

## Controls

- Movement: WASD or Arrow keys
- Interact: E key
- Zoom: Mouse wheel
- Reset camera: R key
- Pause: ESC key

## Features

- Smooth character movement with collision detection
- NPC interaction system with dialogue
- Dynamic sound system with footsteps and ambient music
- Tile-based world system
- Camera controls with zoom
- Resonance field visualization
- Psychological state system

## Project Structure

- `src/` - Source files
  - `core/` - Core systems (resource management, sound, etc.)
  - `entities/` - Entity implementations (player, NPCs)
  - `world.c` - World and room management
  - `game_impl.c` - Main game implementation
- `include/` - Header files
- `resources/` - Game resources
  - `sounds/` - Sound effects and music
  - `sprites/` - Character and object sprites
- `tests/` - Unit tests
- `docs/` - Documentation

## Performance Targets

- Frame rate: 60+ FPS
- Memory usage: <100MB
- Load time: <2 seconds
- Input latency: <16ms

## License

MIT License - See LICENSE file for details 