#ifndef MAP_TYPES_H
#define MAP_TYPES_H

#include <raylib.h>
#include <stdbool.h>
#include "constants.h"

#define CACHE_CHUNK_SIZE 16  // Size of each cached chunk in tiles
#define MAX_CACHED_CHUNKS 64 // Maximum number of chunks to keep in cache

// Forward declarations
struct World;

// Object types that can be placed in the world
typedef enum {
    OBJECT_NONE = 0,
    OBJECT_TREE,
    OBJECT_BUSH,
    OBJECT_FLOWER,
    OBJECT_FOUNTAIN,
    OBJECT_STATUE,
    OBJECT_TORCH,
    OBJECT_ROCK,
    OBJECT_COUNT
} ObjectType;

// Enhanced tile properties (lightweight TMX-like)
typedef struct {
    bool isWalkable;
    bool isDestructible;
    bool isInteractive;
    float friction;
    float resonance;
    Color color;
    const char* customProperties;  // JSON string for custom properties
} TileProperties;

// Basic tile structure
typedef struct {
    TileType type;
    ObjectType objectType;
    TileProperties properties;
} Tile;

// Cached chunk for rendering optimization
typedef struct {
    RenderTexture2D texture;    // Pre-rendered chunk texture
    Rectangle bounds;           // World space bounds of this chunk
    bool isDirty;              // Whether chunk needs updating
    int lastAccessTime;        // For cache eviction
    Vector2 gridPosition;      // Position in chunk grid
} CachedChunk;

// Chunk cache manager
typedef struct {
    CachedChunk chunks[MAX_CACHED_CHUNKS];
    int chunkCount;
    int frameCounter;          // For cache aging
} ChunkCache;

// Viewport for culling
typedef struct {
    Rectangle bounds;          // Visible area in world space
    Vector2 chunkMin;         // Min chunk coordinates visible
    Vector2 chunkMax;         // Max chunk coordinates visible
} Viewport;

// Map layer for rendering
typedef struct {
    Texture2D background;
    Texture2D foreground;
    Texture2D objects;
} RenderLayer;

// Collision grid for efficient collision detection
typedef struct {
    int width;
    int height;
    int cellSize;
    bool* cells;
} CollisionGrid;

// Enhanced map structure
typedef struct {
    Tile* tiles;
    int width;
    int height;
    ChunkCache cache;
    Viewport viewport;
    bool enableCulling;
    TileProperties* properties;  // Array of properties for the map
} TileMap;

#endif // MAP_TYPES_H 