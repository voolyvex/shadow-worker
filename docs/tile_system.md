# Tile System Documentation

## Overview
The Shadow Worker tile system implements an efficient, chunk-based rendering approach with viewport culling and property customization. This system is designed to handle large estate maps while maintaining high performance.

## Architecture

### Resource Pipeline
1. **Individual Tiles**
   - Raw tile images stored in `resources/tiles/`
   - PNG format with transparency support
   - Standard 32x32 pixel size

2. **Tileset Generation**
   - Tiles combined into texture atlases
   - Defined in `resources/maps/tileset_config.json`
   - Supports animations and variants
   - Optimized for GPU memory usage

3. **Runtime Management**
   - ResourceManager handles texture loading/unloading
   - Chunk-based caching system
   - Dynamic texture streaming
   - Memory-efficient tile properties

### Core Components

#### Resource Manager
```c
typedef struct {
    TextureResource* textures;
    size_t textureCount;
    size_t textureCapacity;
} ResourceManager;
```

#### Chunk System
```c
typedef struct {
    RenderTexture2D texture;    // Pre-rendered chunk texture
    Rectangle bounds;           // World space bounds
    bool isDirty;              // Update flag
    int lastAccessTime;        // For LRU cache
    Vector2 gridPosition;      // Grid coordinates
} CachedChunk;
```

#### Tile Properties
```c
typedef struct {
    bool isWalkable;
    bool isDestructible;
    bool isInteractive;
    float friction;
    float resonance;
    Color color;
    const char* customProperties;
} TileProperties;
```

## Performance Optimization

### Memory Management
- Each chunk requires: 16x16 x (tile size) bytes for texture
- Maximum cache size: 64 chunks
- Custom properties are allocated dynamically
- Texture atlas optimization reduces VRAM usage

### Rendering Pipeline
1. **Viewport Calculation**
   - Camera frustum culling
   - Chunk visibility determination
   - Dynamic LOD based on distance

2. **Chunk Management**
   - LRU cache eviction policy
   - Dirty state tracking
   - Asynchronous chunk loading
   - Texture streaming

3. **Draw Optimization**
   - Batched rendering
   - Hardware instancing support
   - Texture atlas UV optimization
   - Minimal state changes

## Integration Guide

### Loading Tiles
```c
// Load tileset configuration
bool LoadTileset(const char* configPath);

// Access tile properties
TileProperties GetTileProperties(TileType type);

// Set custom properties
void SetTileCustomProperties(World* world, int x, int y, const char* props);
```

### Working with Chunks
```c
// Get or create chunk
CachedChunk* GetChunk(ChunkCache* cache, Vector2 gridPos);

// Mark chunk for update
void InvalidateChunk(World* world, int x, int y);

// Force immediate chunk update
void UpdateChunkTexture(MapSystem* mapSystem, CachedChunk* chunk);
```

## Best Practices

1. **Resource Management**
   - Pre-generate texture atlases
   - Use power-of-two textures
   - Implement proper cleanup
   - Cache frequently used tiles

2. **Performance**
   - Keep chunk size power of 2
   - Minimize custom properties
   - Use viewport culling
   - Batch similar tiles

3. **Memory**
   - Monitor VRAM usage
   - Implement texture streaming
   - Use compressed textures
   - Clean up unused resources

## Error Handling

1. **Resource Loading**
   - Validate texture formats
   - Check file existence
   - Handle missing tiles
   - Provide fallback textures

2. **Runtime Checks**
   - Validate chunk bounds
   - Check texture limits
   - Monitor memory usage
   - Handle GPU errors

## Key Features

### Chunk-Based Rendering
- Chunks are 16x16 tile units
- Maximum of 64 cached chunks at any time
- LRU (Least Recently Used) cache eviction policy
- Automatic chunk management based on viewport

### Viewport Culling
- Dynamic viewport tracking based on camera position
- Only renders chunks visible in the current view
- Automatic chunk loading/unloading as viewport moves
- Efficient grid-based chunk calculations

### Tile Properties
- Lightweight TMX-inspired property system
- Custom properties via JSON strings
- Core properties:
  - Walkability
  - Destructibility
  - Interactivity
  - Friction
  - Resonance
  - Color

## Usage Examples

### Setting Custom Tile Properties
```c
// Example JSON property string
const char* props = "{\"sound\":\"creak\",\"light_level\":0.5}";
SetTileCustomProperties(world, x, y, props);
```

### Working with Chunks
```c
// Chunks are automatically managed, but can be manually invalidated
CachedChunk* chunk = GetChunk(&map->cache, gridPos);
if (chunk) {
    chunk->isDirty = true;  // Force chunk update
}
```

## Performance Considerations

### Memory Usage
- Each chunk requires: 16x16 x (tile size) bytes for texture
- Maximum cache size: 64 chunks
- Custom properties are allocated dynamically

### Optimization Tips
1. Keep chunk size (CACHE_CHUNK_SIZE) power of 2
2. Adjust MAX_CACHED_CHUNKS based on map size
3. Use viewport culling for large maps
4. Minimize custom property string sizes

## Technical Details

### Chunk Structure
```c
typedef struct {
    RenderTexture2D texture;    // Pre-rendered chunk texture
    Rectangle bounds;           // World space bounds
    bool isDirty;              // Update flag
    int lastAccessTime;        // For LRU cache
    Vector2 gridPosition;      // Grid coordinates
} CachedChunk;
```

### Viewport Culling
```c
typedef struct {
    Rectangle bounds;          // Visible area
    Vector2 chunkMin;         // Min visible chunk
    Vector2 chunkMax;         // Max visible chunk
} Viewport;
```

## Integration with World System

The tile system integrates with the world system through:
1. Automatic chunk updates during world updates
2. Camera-based viewport tracking
3. Collision detection support
4. Resource management integration

## Future Improvements

Planned enhancements:
1. Multi-threaded chunk updates
2. Compressed texture cache
3. Dynamic chunk size based on distance
4. Property inheritance system 