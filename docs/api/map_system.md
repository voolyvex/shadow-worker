# Map System API Documentation

## Overview
The Map System is responsible for managing the game world's tile-based map, including rendering, collision detection, and object placement. It provides efficient chunk-based rendering and caching mechanisms for optimal performance.

## Core Components

### MapSystem Structure
```c
typedef struct MapSystem {
    TileMap* currentMap;          // Current active map
    CollisionGrid* collisionGrid; // Grid for collision detection
    RenderLayer* layers;          // Array of render layers
    int layerCount;              // Number of active layers
} MapSystem;
```

### TileMap Structure
```c
typedef struct {
    Tile* tiles;                 // Array of tiles
    int width;                   // Map width in tiles
    int height;                  // Map height in tiles
    ChunkCache cache;           // Chunk caching system
    Viewport viewport;          // Current view bounds
    bool enableCulling;         // Whether to use view culling
    TileProperties* properties; // Array of tile properties
} TileMap;
```

## Constants
- `MAX_LAYERS`: Maximum number of map layers (8)
- `CACHE_CHUNK_SIZE`: Size of each cached chunk in tiles (16)
- `MAX_CACHED_CHUNKS`: Maximum number of chunks to keep in cache (64)

## Core Functions

### Map System Management
- `MapSystem* CreateMapSystem(void)`: Creates a new map system
- `void DestroyMapSystem(MapSystem* mapSystem)`: Destroys a map system and frees resources
- `bool LoadMap(MapSystem* mapSystem, const char* fileName)`: Loads a map from file
- `void UpdateMapState(MapSystem* mapSystem, float deltaTime)`: Updates map state
- `void RenderMapLayers(MapSystem* mapSystem)`: Renders all map layers

### Map Object Management
- `void AddMapObject(MapSystem* mapSystem, ObjectType type, Vector2 position)`: Adds an object to the map
- `void RemoveMapObject(MapSystem* mapSystem, Vector2 position)`: Removes an object from the map
- `void UpdateMapObjects(MapSystem* mapSystem, float deltaTime)`: Updates all map objects

### Chunk Management
- `CachedChunk* GetChunk(ChunkCache* cache, Vector2 gridPos)`: Retrieves a chunk from the cache

## Integration with World System
The Map System integrates with the World System through the following functions:
- `void UpdateMapSystem(World* world, float deltaTime)`: Updates map system with world context
- `void DrawMapSystem(MapSystem* mapSystem)`: Renders the map system

## Save/Load System
- `void SaveMapSystem(MapSystem* mapSystem, const char* filename)`: Saves map state to file
- `void LoadMapSystem(MapSystem* mapSystem, const char* filename)`: Loads map state from file

## Memory Management
The Map System uses a chunk-based caching system to optimize memory usage and rendering performance. Chunks are automatically managed and evicted based on:
- Last access time
- Frame counter
- Dirty state (needs update)

## Thread Safety
The Map System is currently single-threaded. All functions should be called from the main thread to ensure proper synchronization with the rendering system.

## Error Handling
The system uses return values and NULL checks to handle errors. Functions that can fail return either a boolean or a pointer that should be checked for NULL.

## Performance Considerations
1. Chunk caching reduces rendering overhead
2. View culling optimizes rendering by only processing visible chunks
3. Dirty flag system prevents unnecessary chunk updates
4. Memory pooling for frequently allocated objects

## Example Usage
```c
// Create and initialize map system
MapSystem* mapSystem = CreateMapSystem();
if (!mapSystem) {
    // Handle error
    return;
}

// Load a map
if (!LoadMap(mapSystem, "level1.map")) {
    // Handle loading error
    DestroyMapSystem(mapSystem);
    return;
}

// Game loop
while (gameRunning) {
    UpdateMapSystem(world, deltaTime);
    DrawMapSystem(mapSystem);
}

// Cleanup
DestroyMapSystem(mapSystem);
``` 