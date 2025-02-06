# Shadow Worker Development Guide

## Project Structure

The project follows a modular architecture with clear separation of concerns:

```
shadow-worker/
├── include/                 # Public headers
│   ├── core/               # Core systems
│   ├── entities/           # Entity management
│   ├── world/             # World and map systems
│   ├── resource/          # Resource management
│   └── utils/             # Utilities
├── src/                    # Implementation files
│   ├── core/              # Core system implementations
│   ├── entities/          # Entity implementations
│   ├── world/             # World system implementations
│   ├── resource/          # Resource management implementations
│   └── utils/             # Utility implementations
├── tests/                  # Test suites
├── docs/                   # Documentation
│   ├── api/               # API documentation
│   ├── guides/            # Development guides
│   └── architecture/      # Architecture documentation
└── resources/             # Game resources
```

## Development Guidelines

### 1. File Organization
- Headers go in `include/` under the appropriate category
- Implementations go in `src/` mirroring the include structure
- Keep related files together in their category directory

### 2. Component System
The component system uses two key elements:
- Component Flags: Define available components (`core/component_flags.h`)
- Component Registry: Manages component storage (`core/component_registry.h`)

### 3. Entity Management
- Entity creation/destruction through entity pool
- Component attachment via component registry
- Clear ownership and lifecycle management

### 4. Resource Management
- Centralized resource loading through resource manager
- Texture management via texture manager
- Sound resources via sound manager

### 5. World System
- World state management in world.h
- Map generation and management
- Entity spawning and management

## Building and Testing

See `testing.md` for detailed testing procedures.
See `validation.md` for validation requirements.

# Development Guides

This section contains comprehensive guides for developing with the Shadow Worker engine.

## Core Development Guides

### [Coding Standards](coding_standards.md)
- Code style and formatting
- Naming conventions
- Documentation requirements
- Code organization

### [Testing Guide](testing.md)
- Test organization
- Writing effective tests
- Test coverage requirements
- Running tests

### [Debugging Guide](debugging.md)
- Debugging tools
- Common issues
- Performance profiling
- Memory analysis

## Workflow Guides

### [Build Process](../workflows/build_process.md)
- Build system setup
- Compilation options
- Dependency management
- Release builds

### [Testing Workflow](../workflows/testing_flow.md)
- Test-driven development
- Continuous integration
- Test automation
- Quality assurance

### [Release Process](../workflows/release_flow.md)
- Version control
- Release preparation
- Documentation updates
- Deployment

## Best Practices

### Code Quality
- Use static analysis tools
- Follow SOLID principles
- Write self-documenting code
- Maintain consistent style

### Testing
- Write tests first
- Cover edge cases
- Test error conditions
- Maintain test documentation

### Performance
- Profile early and often
- Optimize critical paths
- Monitor memory usage
- Benchmark regularly

### Documentation
- Keep docs up to date
- Include examples
- Document assumptions
- Maintain changelog

## Development Environment

### Required Tools
- Visual Studio 2019+
- CMake 3.14+
- Git
- raylib dependencies

### Recommended Tools
- Static analyzers
- Memory checkers
- Profiling tools
- Documentation generators

### Environment Setup
1. Install prerequisites
2. Clone repository
3. Configure build system
4. Set up development tools

## Common Tasks

### Adding New Features
1. Create feature branch
2. Write tests
3. Implement feature
4. Update documentation
5. Submit pull request

### Bug Fixing
1. Create bug report
2. Write failing test
3. Fix bug
4. Verify fix
5. Update documentation

### Code Review
1. Check coding standards
2. Verify test coverage
3. Review documentation
4. Test functionality
5. Provide feedback

## Troubleshooting

### Build Issues
- Clean build directory
- Update dependencies
- Check compiler version
- Verify environment

### Test Failures
- Check test environment
- Verify test data
- Debug test cases
- Review recent changes

### Runtime Issues
- Check log files
- Monitor resources
- Debug with tools
- Profile performance

## Additional Resources

- [Architecture Overview](../architecture/system_design.md)
- [API Documentation](../api/README.md)
- [Example Code](examples.md)
- [Known Issues](known_issues.md) 