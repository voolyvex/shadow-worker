# Shadow Worker - Template System Documentation

## Overview

The template system in Shadow Worker provides a flexible and extensible way to define and generate game environments. It consists of two main components:
1. The Estate Map System
2. The Room Template System

## Estate Map System

### Purpose
The Estate Map System generates the overall layout of the game environment, including:
- Central courtyard
- Connecting paths
- Gardens and water features
- Decorative elements
- NPC spawn points

### Key Components

#### 1. Tile Types
```c
typedef enum {
    TILE_GRASS,    // Base terrain
    TILE_PATH,     // Walkable paths
    TILE_WATER,    // Water features
    TILE_WALL,     // Solid boundaries
    TILE_FLOOR     // Interior surfaces
} TileType;
```

#### 2. Object Types
```c
typedef enum {
    OBJECT_NONE,    // Empty space
    OBJECT_TREE,    // Large vegetation
    OBJECT_BUSH,    // Medium vegetation
    OBJECT_FLOWER,  // Small vegetation
    OBJECT_FOUNTAIN // Central feature
} ObjectType;
```

### Generation Process

1. Base Terrain
   - Initialize grass base layer
   - Define estate boundaries
   - Set up collision properties

2. Core Layout
   - Generate central courtyard
   - Create main pathways
   - Place central fountain
   - Define key architectural elements

3. Environmental Features
   - Add water features
   - Place vegetation
   - Create garden areas
   - Distribute decorative elements

4. Gameplay Elements
   - Define spawn points
   - Place interactive objects
   - Set up collision zones
   - Configure navigation paths

## Tileset Configuration

The tileset configuration (`tileset_config.json`) defines the visual and behavioral properties of all tiles and objects:

```json
{
    "tileSize": 32,
    "tiles": {
        "grass": {
            "id": 0,
            "solid": false,
            "coords": [0, 0]
        },
        // ... other tile definitions
    },
    "objects": {
        "tree": {
            "id": 1,
            "solid": true,
            "coords": [1, 1]
        },
        // ... other object definitions
    },
    "animations": {
        "water_ripple": {
            "frames": [[2, 0], [2, 1], [2, 2]],
            "duration": 1.0
        }
    }
}
```

## Integration with World System

The Estate Map System integrates with the World System through:

1. Tile Management
```c
void SetTileAt(World* world, int x, int y, TileType type);
TileType GetTileAt(World* world, int x, int y);
```

2. Spawn Point Management
```c
bool IsValidSpawnPoint(World* world, Vector2 position);
Vector2 GetRandomSpawnPoint(World* world);
```

3. Collision Detection
```c
bool IsTileSolid(World* world, int x, int y);
bool CheckCollision(World* world, Rectangle bounds);
```

## Memory Management

The system employs careful memory management:

1. Allocation
```c
EstateMap* CreateEstateMap(World* world);
```

2. Cleanup
```c
void UnloadEstateMap(EstateMap* map);
```

## Testing

The template system includes comprehensive testing:

1. Unit Tests
```c
void AssertInBounds(void);
void AssertGetIndex(void);
void ValidateMapCreation(void);
void ValidateMapGeneration(void);
```

2. Integration Tests
- World system integration
- Resource management
- Memory leak detection
- Performance benchmarks

## Future Enhancements

1. Procedural Generation
   - Enhanced path algorithms
   - More varied garden layouts
   - Dynamic object placement
   - Weather-based variations

2. Visual Effects
   - Dynamic lighting
   - Particle systems
   - Weather effects
   - Time-of-day changes

3. Gameplay Features
   - Interactive objects
   - Dynamic events
   - Environmental hazards
   - Seasonal changes

4. Performance Optimizations
   - Tile batching
   - Occlusion culling
   - Memory pooling
   - Asset streaming

## Template Types

### Room Templates
```json
{
    "type": "combat",
    "difficulty": 1,
    "size": {
        "x": 6,
        "y": 6
    },
    "connections": {
        "positions": [
            {
                "x": 0,
                "y": 3,
                "type": "door",
                "roomSize": {"x": 6, "y": 6}
            }
        ]
    },
    "spawns": [
        {
            "x": 3,
            "y": 3,
            "type": "enemy",
            "enemyType": "basic"
        }
    ]
}
```

### Supported Room Types
- `combat`: Combat encounter rooms
- `treasure`: Reward and item rooms
- `shop`: Merchant and upgrade rooms
- `boss`: Boss battle arenas

### Room Sizes
- `tiny`: 4x4 tiles
- `small`: 6x6 tiles
- `medium`: 8x8 tiles
- `large`: 10x10 tiles
- `huge`: 12x12 tiles
- `boss`: 16x16 tiles

## Scaling System

### Scaling Modes
1. **PixelPerfect**
   - Nearest-neighbor scaling
   - No interpolation
   - Best for maintaining pixel art clarity

2. **Smooth**
   - Bilinear filtering
   - Mipmapping enabled
   - Suitable for larger rooms

3. **Detailed**
   - Trilinear filtering
   - Anisotropic filtering (4x)
   - Best for high-resolution assets

### Feature Preservation
The scaling system preserves:
- Door positions and alignment
- Spawn point validity
- Obstacle placement
- Room proportions
- Gameplay balance

## Integration System

### Door Alignment
- Doors must be placed on room edges
- Center alignment within wall sections
- Minimum spacing between doors
- Valid clearance for player movement

### Transitions
1. **Smooth Transitions**
   - Gradual blending between rooms
   - Height matching for seamless connections
   - Texture blending at boundaries

2. **Sharp Transitions**
   - Immediate changes between rooms
   - Useful for distinct area separation
   - Maintains architectural integrity

### Connection Matching
The system scores connections based on:
- Relative position (40% weight)
- Orientation compatibility (30% weight)
- Size compatibility (30% weight)

## Usage Examples

### Basic Template Validation
```powershell
Test-LevelTemplate -TemplatePath "templates/combat_room.json" -TemplateType "rooms"
```

### Scaling Templates
```powershell
$scaled = Get-ScaledTemplate -TemplatePath "templates/basic_room.json" -TargetSize "large" -ScaleMode "PixelPerfect"
```

### Creating Transitions
```powershell
$transition = New-TemplateTransition -SourceTemplatePath "room1.json" -TargetTemplatePath "room2.json" -TransitionType "smooth"
```

## Performance Optimization

### Memory Management
- Template caching for frequent sizes
- Resource pooling for common elements
- Efficient texture atlas usage
- Dynamic batch processing

### Loading Optimization
- Chunk-based loading system
- Asynchronous template processing
- Background transition generation
- Smart resource unloading

## Validation Rules

### Room Templates
- Must specify type and difficulty
- Valid size dimensions
- At least one connection point
- Proper spawn point placement

### Transitions
- Compatible connection points
- Valid blend distances
- Proper texture coordinates
- Collision boundary integrity

## Testing

### Running Tests
```powershell
.\tests\test_template_scaling.ps1 -Verbose
```

### Test Coverage
- Template validation
- Scaling accuracy
- Feature preservation
- Transition generation
- Performance metrics

## Best Practices

1. **Template Design**
   - Use standard room sizes
   - Place doors at standard positions
   - Maintain clear paths
   - Balance feature distribution

2. **Scaling Considerations**
   - Choose appropriate scaling modes
   - Verify feature preservation
   - Test different size combinations
   - Monitor performance impact

3. **Integration Guidelines**
   - Plan connection points carefully
   - Test transitions thoroughly
   - Validate gameplay flow
   - Consider visual coherence

## Error Handling

### Common Issues
1. Invalid template structure
2. Incompatible scaling requests
3. Connection matching failures
4. Resource loading errors

### Resolution Steps
1. Validate template format
2. Check size compatibility
3. Verify connection points
4. Monitor resource usage

## Future Enhancements

### Planned Features
1. Advanced biome transitions
2. Dynamic difficulty scaling
3. Procedural decoration
4. Enhanced lighting integration

### Performance Improvements
1. Optimized batch processing
2. Enhanced caching system
3. Smarter resource management
4. Parallel processing support 