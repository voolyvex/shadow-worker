# Test Directory Structure

## Current Test Status

⚠️ **Note:** Resource manager tests are temporarily disabled during the prototype phase.

## Directory Organization

```
tests/
├── include/              # Test headers
│   └── test_suites.h    # Test suite declarations
├── suites/              # Test implementation files
│   ├── test_core_systems.c
│   ├── test_world.c
│   ├── test_memory.c
│   ├── test_map.c
│   ├── test_texture_manager.c  # Added test for TextureManager
│   └── test_resource_manager.c
├── resources/           # Test-specific resources
│   ├── maps/           # Test map files
│   └── sprites/        # Test sprite resources
│       ├── player/     # Player test sprites
│       └── npc/        # NPC test sprites
├── test_main.c         # Main test runner
├── test_env.c          # Test environment setup
└── CMakeLists.txt      # Test build configuration

Note: The actual game resources are stored in the root 'resources/' directory with the following structure:
resources/
├── sprites/            # Character and entity sprites
│   ├── player/        # Player character sprites
│   └── npc/           # NPC character sprites
├── tiles/             # Tileset resources
├── buildings/         # Building sprites and assets
├── objects/           # Object sprites and assets
├── maps/             # Map data files
├── sounds/           # Sound effects and music
├── shaders/          # Shader programs
└── fonts/            # Font resources

## Test Categories

1. **Core Systems Tests**
   - Basic system functionality
   - Memory management
   - Resource handling (temporarily disabled)

2. **Integration Tests**
   - Component interaction
   - System communication
   - Resource lifecycle

3. **Map Tests**
   - Map generation
   - Tile system
   - World layout

4. **Performance Tests**
   - Memory usage
   - Resource limits
   - System benchmarks

## Temporarily Disabled Tests

The following tests are temporarily disabled during the prototype phase:
- Resource manager tests (see GitHub issue #XXX)
- Related texture management tests

These tests will be re-enabled once the resource management system is stabilized.

## Running Tests

```bash
# Build and run all tests
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure

# Run specific test suite
./build/Debug/sw_test_suite
```

## Adding New Tests

1. Create test file in appropriate directory
2. Add to test_suites.h
3. Register in CMakeLists.txt
4. Document in this file
