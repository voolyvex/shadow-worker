# Build Process Documentation

## Updated Build Structure

```
shadow-worker/
├── build/                  # Unified build directory
│   ├── bin/                # Executables and runtime files
│   ├── lib/                # Libraries and archives
│   └── tests/              # Test executables and resources
├── src/                    # Source code
├── include/                # Header files
├── tests/                  # Test suites
└── resources/              # Game assets
```

## Key Changes

1. **Single Build Directory**: All build artifacts are now consolidated in the `build` directory
2. **Standardized Output**: Clear separation of binaries, libraries, and test resources
3. **Simplified Workflow**: Single command to build and test the project
4. **Multi-configuration Support**: Proper handling of Debug/Release configurations

## Build Commands

```bash
# Clean build
build_and_test.bat

# Or manually:
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
ctest -C Debug --output-on-failure
```

## Test Environment

- Test resources are automatically copied to `build/tests/resources`
- Test executables are placed in `build/bin`
- Test working directory is set to `build/tests`

## Migration Guide

### For Developers

1. Delete existing `shadow_workspace` directory
2. Update your local repository:
   ```bash
   git fetch origin
   git checkout consolidated-build
   ```
3. Clean existing build artifacts:
   ```bash
   build_and_test.bat clean
   ```
4. Build and test:
   ```bash
   build_and_test.bat
   ```

### For CI/CD Pipelines

Update build scripts to use the new unified build directory:

```yaml
steps:
  - name: Build and Test
    run: |
      mkdir build
      cd build
      cmake .. -G "Visual Studio 17 2022" -A x64
      cmake --build . --config Debug
      ctest -C Debug --output-on-failure
```

## Troubleshooting

### Common Issues

1. **Build Failures**
   - Verify CMake version (>= 3.14)
   - Check Visual Studio installation
   - Ensure all dependencies are installed

2. **Test Failures**
   - Verify test resources are copied correctly
   - Check working directory for tests
   - Validate environment variables

3. **Resource Loading Issues**
   - Verify resource paths
   - Check file permissions
   - Ensure resources are included in build

## Maintenance

### Regular Tasks

1. Clean build artifacts:
   ```bash
   build_and_test.bat clean
   ```
2. Update dependencies:
   ```bash
   git submodule update --init --recursive
   ```
3. Run full test suite:
   ```bash
   build_and_test.bat test
   ```

### Version Control

1. Commit changes to feature branch
2. Update documentation
3. Run build verification
4. Create pull request 