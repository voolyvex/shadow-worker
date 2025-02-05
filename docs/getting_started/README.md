# Getting Started with Shadow Worker

This guide will help you get up and running with the Shadow Worker project quickly.

## Prerequisites

Before you begin, ensure you have:

- CMake 3.14 or higher
- C11 compatible compiler
- Git for version control
- Visual Studio 2019 or later (Windows)
- raylib dependencies (automatically handled by CMake)

## Quick Start

1. **Clone the Repository**
   ```bash
   git clone --recursive https://github.com/yourusername/shadow-worker.git
   cd shadow-worker
   ```

2. **Build the Project**
   ```bash
   # Using the build script (recommended)
   build_and_test.bat

   # Or manually
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Debug
   ```

3. **Run Tests**
   ```bash
   cd build
   ctest --output-on-failure
   ```

4. **Run the Application**
   ```bash
   # From build directory
   Debug\ShadowWorker.exe
   ```

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
├── resources/         # Game assets
└── external/          # External dependencies
```

## Current Status

⚠️ **Note:** Project is currently in prototype phase. Some features may be incomplete or subject to change.

### Available Features
- Basic window management
- Core system architecture
- Logging system
- Basic resource management
- Test framework

### In Development
- Resource management system optimization
- Memory management improvements
- Test suite stabilization

## Next Steps

1. Review the [Architecture Overview](../architecture/system_design.md)
2. Check the [Development Guide](../guides/README.md)
3. Read the [API Documentation](../api/README.md)
4. Look at [Example Code](../guides/examples.md)

## Common Issues

### Build Issues
- Ensure all prerequisites are installed
- Check CMake version
- Verify Visual Studio installation
- Clean build directory and retry

### Runtime Issues
- Verify resource paths
- Check log files
- Ensure proper initialization
- Monitor memory usage

## Getting Help

- Check the [Troubleshooting Guide](../guides/troubleshooting.md)
- Review [Known Issues](../guides/known_issues.md)
- Search the [Issue Tracker](https://github.com/yourusername/shadow-worker/issues)
- Join the [Discussion Forum](https://github.com/yourusername/shadow-worker/discussions)

## Contributing

See our [Contributing Guide](../CONTRIBUTING.md) for:
- Code style guidelines
- Pull request process
- Development workflow
- Testing requirements 