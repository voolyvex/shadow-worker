# Shadow Worker Documentation

## Project Structure

```
shadow-worker/
├── include/                 # Public headers
│   ├── core/               # Core system headers
│   ├── entities/           # Entity system headers
│   ├── world/             # World and map headers
│   ├── resource/          # Resource management headers
│   └── utils/             # Utility headers
├── src/                    # Implementation files
│   ├── core/              # Core system implementations
│   ├── entities/          # Entity implementations
│   ├── world/             # World system implementations
│   ├── resource/          # Resource implementations
│   └── utils/             # Utility implementations
├── tests/                  # Test suite
│   ├── include/           # Test headers
│   ├── suites/           # Test implementations
│   └── resources/         # Test-specific resources
├── docs/                   # Documentation
│   ├── api/               # API documentation
│   ├── guides/            # Development guides
│   └── architecture/      # Architecture documentation
└── resources/             # Game resources
```

## Documentation Structure

1. **API Documentation** (`api/`)
   - Complete API reference
   - Function documentation
   - Type definitions
   - Usage examples

2. **Development Guides** (`guides/`)
   - Getting started
   - Build instructions
   - Testing procedures
   - Best practices

3. **Architecture** (`architecture/`)
   - System design
   - Component relationships
   - Data flow
   - Performance considerations

## Key Systems

1. **Core Systems** (`include/core/`)
   - Component management
   - Memory handling
   - Basic utilities

2. **Entity System** (`include/entities/`)
   - Entity management
   - Component data
   - Entity behaviors

3. **World System** (`include/world/`)
   - World state
   - Map management
   - Physics and collision

4. **Resource Management** (`include/resource/`)
   - Asset loading
   - Resource caching
   - Memory management

## Building and Testing

See `guides/README.md` for detailed build instructions and testing procedures.

## Quick Links

- [Getting Started](getting_started/README.md)
- [Architecture Overview](architecture/system_design.md)
- [API Reference](api/README.md)
- [Development Guides](guides/README.md)

## Current Status

⚠️ **Note:** Project is currently in prototype phase. Some documentation may be incomplete or subject to change.

## Documentation Guidelines

1. **Organization**
   - Each major component has its own directory
   - Each directory contains a README.md for overview
   - API documentation follows standard format
   - Examples included where appropriate

2. **Maintenance**
   - Documentation updated with code changes
   - Examples tested and verified
   - Links checked regularly
   - Version numbers kept current

3. **Style**
   - Clear and concise language
   - Consistent formatting
   - Code examples in appropriate language blocks
   - Screenshots and diagrams where helpful

## Contributing

See [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines on:
- Documentation style guide
- How to submit changes
- Documentation review process
- Building documentation locally

## Version Control

- Documentation versions match code releases
- Breaking changes clearly marked
- Deprecated features noted
- Migration guides provided

## Getting Help

- [Issue Tracker](https://github.com/yourusername/shadow-worker/issues)
- [Discussion Forum](https://github.com/yourusername/shadow-worker/discussions)
- [Development Wiki](https://github.com/yourusername/shadow-worker/wiki) 