#ifndef SHADOW_WORKER_WORLD_H
#define SHADOW_WORKER_WORLD_H

#include <raylib.h>
#include <stdbool.h>
#include "resource_manager.h"
#include "entity.h"

// World configuration
#define ESTATE_WIDTH 50  // Larger than screen width
#define ESTATE_HEIGHT 40 // Larger than screen height
#define TILE_SIZE 32
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define MAX_SPAWN_POINTS 32

// Forward declarations
typedef struct World World;

// Tile types for estate
typedef enum TileType {
    TILE_GRASS,
    TILE_PATH,
    TILE_WATER,
    TILE_WALL,
    TILE_FLOOR,
    TILE_COLUMN,
    TILE_TREE,
    TILE_BUSH,
    TILE_FLOWER,
    TILE_FOUNTAIN,
    TILE_STATUE,
    TILE_COUNT
} TileType;

// Tile properties
typedef struct TileProperties {
    bool isSolid;
    bool isWater;
    bool isInteractive;
    Rectangle textureRect;  // Source rectangle in tileset
    const char* textureName;
} TileProperties;

// World structure
typedef struct World {
    TileType* tiles;
    TileProperties* tileProperties;
    Vector2 dimensions;
    EntityPool* entityPool;
    ResourceManager* resources;
    float deltaTime;
    Camera2D* camera;
    Rectangle bounds;
    float globalResonance;
    float instabilityLevel;
    bool isStable;
    Vector2* spawnPoints;      // NPC spawn locations
    int spawnPointCount;       // Number of spawn points
} World;

// Function declarations
World* InitWorld(ResourceManager* resources);
void UpdateWorld(World* world, float deltaTime);
void DrawWorld(World* world);
void UnloadWorld(World* world);

// Tile management
TileType GetTileAt(World* world, int x, int y);
void SetTileAt(World* world, int x, int y, TileType type);
bool IsTileSolid(World* world, int x, int y);
Vector2 GetTilePosition(int x, int y);
void GenerateEstateMap(World* world);

// Spawn point management
bool IsValidSpawnPoint(World* world, Vector2 position);
Vector2 GetRandomSpawnPoint(World* world);

// Collision handling
bool CheckCollision(World* world, Rectangle bounds);

// Debug
void DebugDrawWorld(World* world);

// Global world instance getter
World* GetWorld(void);

#endif // SHADOW_WORKER_WORLD_H 