# Shadow Worker

A modern game engine built with C and raylib, focusing on efficient resource management and dynamic world generation.

## Current Status

⚠️ **Note:** Project is currently in prototype phase. Resource management tests are temporarily disabled (see GitHub issue #XXX).

## Prerequisites

- CMake 3.14 or higher
- C11 compatible compiler
- Git for version control
- Visual Studio 2019 or later (Windows)
- raylib dependencies (automatically handled by CMake)

## Building the Project

### Windows

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/yourusername/shadow-worker.git
cd shadow-worker

# Build using the batch script
build_and_test.bat
```

Or manually:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

Note: Some tests are temporarily disabled during the prototype phase.

## Project Structure

```
shadow-worker/
├── src/                 # Source code
│   ├── core/           # Core systems
│   ├── entity/         # Entity system
│   └── resource/       # Resource management
├── include/            # Header files
├── tests/              # Test suites
├── docs/              # Documentation
│   ├── architecture/  # System design docs
│   ├── api/          # API documentation
│   └── workflows/    # Build and dev workflows
├── resources/         # Game assets
└── external/          # External dependencies
```

## Documentation

- [Architecture Overview](docs/architecture/system_design.md)
- [Resource Management](docs/resource_management.md)
- [Build Process](docs/workflows/build_process.md)
- [Test Structure](docs/test_dir_structure.md)

## Development Status

### Completed Features
- Basic window management
- Core system architecture
- Logging system
- Basic resource management
- Test framework setup

### In Progress
- Resource management system optimization
- Memory management improvements
- Test suite stabilization

### Planned Features
- Advanced resource pooling
- Async resource loading
- Hot reloading support
- Performance optimization

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Known Issues

- Resource manager tests temporarily disabled (Issue #XXX)
- Memory leak detection pending implementation
- Some test suites need stabilization

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- raylib - Excellent game programming library
- CMake - Build system generator
- Contributors and testers

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

## Testing

The Shadow Worker project uses a comprehensive testing suite to ensure code quality and functionality. Tests are organized into different suites covering core systems, world mechanics, memory management, and map functionality.

### Running Tests

There are several ways to run the tests:

1. **Using the Build Script (Recommended)**
   ```batch
   build_and_test.bat
   ```
   This batch file will:
   - Create and configure the build directory
   - Build the project in Debug mode
   - Run all tests with detailed output
   - Launch the game if tests pass

2. **Manual Test Execution**
   ```batch
   mkdir build && cd build
   cmake -G "Visual Studio 17 2022" -A x64 ..
   cmake --build . --config Debug
   ctest -C Debug --output-on-failure
   ```

3. **Running Specific Test Suites**
   The test executable can be run directly to see more detailed output:
   ```batch
   .\build\bin\Debug\sw_test_suite.exe
   ```

### Test Structure

```
tests/
├── include/              # Test headers
│   └── test_suites.h    # Test suite declarations
├── suites/              # Test implementation files
│   ├── test_core_systems.c
│   ├── test_world.c
│   ├── test_memory.c
│   └── test_map.c
├── resources/           # Test-specific resources
├── test_main.c         # Main test runner
└── test_env.c          # Test environment setup
```

### Adding New Tests

1. Create a new test file in `tests/suites/`
2. Add the test suite declaration to `tests/include/test_suites.h`
3. Add the test suite execution in `tests/test_main.c`
4. Any test resources should be placed in `tests/resources/`

### Test Coverage

When building with GCC, test coverage information is automatically generated. Coverage reports can be found in the build directory after test execution.

## Known Issues

### Testing Status

Currently, the resource manager tests are temporarily disabled due to stability issues. This is being tracked in GitHub issue #XXX. The remaining test suites are functioning normally.

#### Temporary Workarounds

- Resource manager tests are skipped in the test suite
- Core functionality remains tested through integration tests
- Manual testing of resource management is required until issue resolution

#### Development Plan

1. Continue development using remaining test suites
2. Resource manager tests will be fixed in a separate branch
3. Track progress in GitHub issue #XXX
4. Re-enable tests once stability issues are resolved

For developers:
- Use manual resource validation when adding new resources
- Document any resource-related issues in GitHub issue #XXX
- Follow resource management best practices in `docs/resource_management.md`

 