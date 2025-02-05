# Shadow Worker API Documentation

## Overview

The Shadow Worker API is organized into several core systems that work together to provide game functionality.

## Core Systems

### Resource Management
- [Resource Manager](resource_manager.md)
  - Resource loading and unloading
  - Memory management
  - Asset caching
  - Reference counting

### World System
- [Map System](map_system.md)
  - Tile management
  - World generation
  - Chunk loading
  - Collision detection

### Entity System
- [Entity System](entity_system.md)
  - Entity creation and destruction
  - Component management
  - Entity queries
  - Update systems

## API Design Principles

1. **Simplicity**
   - Clear function names
   - Consistent parameter ordering
   - Minimal dependencies
   - Well-defined responsibilities

2. **Safety**
   - Null checking
   - Bounds validation
   - Resource cleanup
   - Error handling

3. **Performance**
   - Efficient memory use
   - Cache-friendly design
   - Minimal allocations
   - Batch processing

4. **Extensibility**
   - Modular design
   - Plugin architecture
   - Event system
   - Custom components

## Common Patterns

### Resource Management
```c
// Initialization
ResourceManager* manager = GetResourceManager();
if (!manager || !manager->initialized) {
    // Handle error
}

// Resource Loading
bool success = LoadGameTexture(manager, "path/to/texture.png", "texture_key");
if (!success) {
    // Handle error
}

// Resource Access
const Texture2D* texture = GetGameTexture(manager, "texture_key");
if (!texture) {
    // Handle missing resource
}

// Cleanup
UnloadGameTexture(manager, "texture_key");
```

### Error Handling
```c
// Function return values indicate success/failure
bool result = SystemFunction();
if (!result) {
    LOG_ERROR(LOG_CORE, "System function failed");
    return false;
}

// Pointer returns are checked for NULL
void* data = GetResource();
if (!data) {
    LOG_ERROR(LOG_CORE, "Failed to get resource");
    return NULL;
}
```

## Status Codes

| Code | Meaning | Description |
|------|---------|-------------|
| 0    | Success | Operation completed successfully |
| -1   | Error   | General error condition |
| -2   | Invalid | Invalid parameter or state |
| -3   | Memory  | Memory allocation failure |
| -4   | IO      | Input/output error |

## Thread Safety

- Resource Manager: Thread-safe for reading
- Entity System: Single-threaded
- Map System: Thread-safe for chunk loading
- Event System: Thread-safe

## Best Practices

1. **Resource Management**
   - Always check initialization
   - Clean up resources in reverse order
   - Use RAII where possible
   - Monitor memory usage

2. **Error Handling**
   - Check return values
   - Log errors with context
   - Clean up on failure
   - Provide error details

3. **Performance**
   - Batch similar operations
   - Minimize allocations
   - Use appropriate data structures
   - Profile critical paths

4. **Testing**
   - Unit test all public APIs
   - Test error conditions
   - Verify memory management
   - Check thread safety

## API Versioning

- Major version: Breaking changes
- Minor version: New features
- Patch version: Bug fixes
- API version in header files

## Documentation Format

Each API function should be documented with:
- Brief description
- Parameters and types
- Return value
- Error conditions
- Usage example
- Thread safety notes 