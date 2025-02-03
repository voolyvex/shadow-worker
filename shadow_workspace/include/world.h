#ifndef WORLD_H
#define WORLD_H

#include <raylib.h>
#include <stdbool.h>
#include "entity_types.h"
#include "entity_pool.h"
#include "map_types.h"
#include "map_system.h"
#include "resource_manager.h"
#include "constants.h"

// Forward declarations
struct EntityPool;
struct ComponentRegistry;
struct MapSystem;
struct TileProperties;

// World textures structure
typedef struct WorldTextures {
    Texture2D tilesetTexture;
    Texture2D playerTexture;
    Texture2D npcTexture;
} WorldTextures;

// World structure
typedef struct World {
    Vector2 dimensions;
    float gravity;
    float friction;
    int width;
    int height;
    int tileSize;
    int spawnCount;
    Vector2 spawnPoints[MAX_SPAWN_POINTS];
    struct TileProperties* tileProperties;
    Camera2D camera;
    WorldTextures textures;
    Tile* tiles;
    ResourceManager* resourceManager;
    EntityPool* entityPool;
} World;

// World state structure
typedef struct WorldState {
    World* world;
    EntityPool* entityPool;
    ComponentRegistry* registry;
    MapSystem* mapSystem;
    ResourceManager* textureManager;
    Camera2D camera;
} WorldState;

// World management functions
WorldState* CreateWorldState(void);
void DestroyWorldState(WorldState* state);
void UnloadWorldState(WorldState* state);
void DestroyWorldStateAndResources(WorldState* state);

// World state management
void UpdateWorld(WorldState* state, float deltaTime);
void DrawWorld(WorldState* state);

// Object management
void AddObject(World* world, ObjectType type, Vector2 position);
void RemoveObject(World* world, Vector2 position);
void UpdateObjects(World* world, float deltaTime);

// World state management
void SaveWorldState(WorldState* state, const char* filename);
void LoadWorldState(WorldState* state, const char* filename);

// Map system functions
void UpdateMapSystem(MapSystem* mapSystem, float deltaTime);
void DrawMapSystem(MapSystem* mapSystem);
void AddMapObject(MapSystem* mapSystem, ObjectType type, Vector2 position);
void RemoveMapObject(MapSystem* mapSystem, Vector2 position);
void UpdateMapObjects(MapSystem* mapSystem, float deltaTime);
void SaveMapSystem(MapSystem* mapSystem, const char* filename);
void LoadMapSystem(MapSystem* mapSystem, const char* filename);

// Tile management
void SetTile(World* world, int x, int y, TileType tileType);
TileType GetTile(World* world, int x, int y);
bool IsWalkable(World* world, int x, int y);

// Spawn point management
void AddSpawnPoint(World* world, Vector2 position);
Vector2 GetRandomSpawnPoint(World* world);

// Debug functions
void DrawWorldDebug(World* world);
void DrawCollisionDebug(World* world);

// World state management
World* CreateWorld(int width, int height, float gravity, ResourceManager* resourceManager);
void DestroyWorld(World* world);

// Tile management
void SetTileAt(World* world, int x, int y, Tile tile);
Tile GetTileAt(World* world, int x, int y);

#endif // WORLD_H