# Shadow Worker API Documentation

## Project Structure

The codebase is organized into the following main categories:

### Core Systems (`include/core/`)
- `component_flags.h`: Component type definitions and bit flags
- `component_registry.h`: Component storage and management system
- `logger.h`: Logging utilities
- `warning_suppression.h`: Warning management

### Entity System (`include/entities/`)
- `entity.h`: Entity management and manipulation
- `entity_pool.h`: Entity pooling and lifecycle
- `entity_types.h`: Entity component structures
- `player.h`: Player-specific functionality
- `npc.h`: NPC behavior and management

### World System (`include/world/`)
- `world.h`: World state and management
- `world_gen.h`: Procedural world generation
- `map_system.h`: Map management and operations
- `map_types.h`: Map data structures
- `estate_map.h`: Estate-specific map functionality

### Resource Management (`include/resource/`)
- `resource_manager.h`: Resource management system
- `resource_types.h`: Resource type definitions
- `texture_manager.h`: Texture loading and management
- `texture_atlas.h`: Texture atlas system
- `sound_manager.h`: Sound resource management

### Utilities (`include/utils/`)
- `debug.h`: Debugging utilities
- `constants.h`: Global constants
- `object_types.h`: Basic object type definitions

## Component System

The component system is built on two main pillars:
1. Component Flags (`component_flags.h`): Defines WHAT components exist
2. Component Registry (`component_registry.h`): Manages HOW components are stored

### Component Types
Each component serves a specific purpose:
- Transform: Position, rotation, scale
- Physics: Movement, forces, collisions
- Render: Visual representation
- Collider: Collision detection
- AI: NPC behavior
- Player: Player-specific controls

### Usage Example
```c
// Create an entity with transform and render components
Entity* entity = CreateEntity();
AddComponent(entity, COMPONENT_TRANSFORM | COMPONENT_RENDER);

// Initialize components
TransformComponent* transform = GetTransformComponent(entity);
InitializeTransformComponent(transform, (Vector2){0, 0});

RenderComponent* render = GetRenderComponent(entity);
InitializeRenderComponent(render);
```

## Build System
The project uses CMake for build configuration. Source files are organized to mirror the include directory structure:

```
src/
├── core/
├── entities/
├── world/
├── resource/
└── utils/
```

Each implementation file corresponds to its header in the include directory.

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