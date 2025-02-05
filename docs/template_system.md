# Shadow Worker - Template System

## Overview

The template system provides a flexible framework for procedural room generation and texture management. It integrates with the tile system to create dynamic, visually rich environments.

## Core Components

### 1. Room Templates
```c
typedef struct RoomTemplate {
    int width;
    int height;
    TileType** tiles;
    ObjectType** objects;
    Vector2* spawnPoints;
    int spawnCount;
    Rectangle* connections;
    int connectionCount;
    MapSystem mapSystem;
} RoomTemplate;
```

### 2. Tile Types
```c
typedef enum {
    TILE_EMPTY = 0,
    TILE_FLOOR = 1,
    TILE_WALL = 2,
    TILE_DOOR = 3,
    TILE_GRASS = 4,
    TILE_WATER = 5
} TileType;
```

### 3. Object Types
```c
typedef enum {
    OBJECT_NONE = 0,
    OBJECT_CHEST = 1,
    OBJECT_TORCH = 2,
    OBJECT_TREE = 3,
    OBJECT_BUSH = 4
} ObjectType;
```

## Resource Integration

### 1. Texture Management
```json
{
    "tileSize": 32,
    "tiles": {
        "empty": {
            "id": 0,
            "solid": false,
            "textureCoords": [0, 0]
        },
        "floor": {
            "id": 1,
            "solid": false,
            "textureCoords": [1, 0]
        }
    },
    "animations": {
        "water_ripple": {
            "frames": [[2, 0], [2, 1], [2, 2]],
            "duration": 1.0
        }
    }
}
```

### 2. Resource Loading Pipeline
1. Load individual tile textures
2. Generate texture atlas
3. Configure tile properties
4. Initialize room templates
5. Set up animation data

## Generation Pipeline

### 1. Template Loading
```c
// Load and validate template
RoomTemplate* LoadRoomTemplate(const char* path) {
    // Parse JSON template
    // Validate dimensions
    // Initialize arrays
    // Set up MapSystem
    return template;
}
```

### 2. Room Generation
```c
// Generate room from template
bool GenerateRoom(World* world, RoomTemplate* template, Vector2 position) {
    // Validate placement
    // Copy tiles and objects
    // Set up connections
    // Initialize spawn points
    return success;
}
```

### 3. World Integration
```c
// Integrate room into world
void IntegrateRoom(World* world, Room* room) {
    // Update collision data
    // Set up lighting
    // Initialize objects
    // Configure properties
}
```

## Texture Atlas Generation

### 1. Atlas Configuration
```json
{
    "atlas": {
        "width": 512,
        "height": 512,
        "tileSize": 32,
        "padding": 1,
        "format": "RGBA8888"
    }
}
```

### 2. Generation Process
1. Scan tile directory
2. Load individual textures
3. Pack into atlas
4. Generate UV coordinates
5. Save atlas metadata

### 3. Runtime Usage
```c
// Access atlas textures
void DrawTile(TileType type, Vector2 position) {
    Rectangle source = GetTileSourceRect(type);
    DrawTextureRec(atlas, source, position, WHITE);
}
```

## Template Format

### 1. Room Definition
```json
{
    "name": "basic_room",
    "type": "ROOM_BASE",
    "size": {
        "width": 10,
        "height": 8
    },
    "layers": {
        "tiles": [[2,2,2,2,2], [2,1,1,1,2], [2,2,2,2,2]],
        "objects": [[0,0,0,0,0], [0,1,0,2,0], [0,0,0,0,0]]
    },
    "spawns": [
        {"type": "npc", "x": 5, "y": 4}
    ],
    "connections": [
        {"side": "west", "y": 3},
        {"side": "east", "y": 3}
    ]
}
```

### 2. Texture Properties
```json
{
    "properties": {
        "resonanceEnabled": true,
        "shadowCasting": true,
        "ambientOcclusion": true,
        "waterReflections": true
    }
}
```

## Performance Optimization

### 1. Memory Management
- Pre-generate texture atlases
- Cache frequently used templates
- Implement resource streaming
- Use compressed textures

### 2. Runtime Optimization
- Batch similar draw calls
- Implement texture atlasing
- Use hardware instancing
- Minimize state changes

### 3. Resource Loading
- Asynchronous template loading
- Progressive texture loading
- Background atlas generation
- Resource pooling

## Error Handling

### 1. Template Validation
```c
bool ValidateTemplate(const RoomTemplate* template) {
    // Check dimensions
    // Validate connections
    // Verify textures
    // Check properties
}
```

### 2. Resource Verification
```c
bool VerifyResources(const RoomTemplate* template) {
    // Check texture availability
    // Validate atlas integrity
    // Verify memory constraints
    // Check file access
}
```

## Best Practices

1. **Template Design**
   - Keep templates modular
   - Use consistent naming
   - Document constraints
   - Version control templates

2. **Resource Management**
   - Pre-generate atlases
   - Cache common templates
   - Clean up unused resources
   - Monitor memory usage

3. **Error Recovery**
   - Provide fallback textures
   - Handle missing resources
   - Log validation errors
   - Implement safe defaults 