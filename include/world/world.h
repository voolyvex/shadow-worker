#ifndef SHADOW_WORKER_WORLD_H
#define SHADOW_WORKER_WORLD_H

#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include "entities/entity_types.h"
#include "entities/entity_pool.h"
#include "core/component_registry.h"
#include "resource/texture_manager.h"
#include "utils/constants.h"
#include "utils/object_types.h"
#include "world/map_system.h"
#include "world/map_types.h"

// Forward declarations
struct EntityPool;
struct ComponentRegistry;
struct MapSystem;
struct ResourceManager;

#define MAX_SPAWN_POINTS 16

// World textures structure
typedef struct {
    Texture2D tilesetTexture;
    Texture2D playerTexture;
    Texture2D npcTexture;
} WorldTextures;

// World structure
typedef struct {
    Vector2 dimensions;
    float gravity;
    float friction;
    int width;
    int height;
    int tileSize;
    int spawnPointCount;
    Vector2 spawnPoints[MAX_SPAWN_POINTS];
    TileProperties* tileProperties;
    Camera2D camera;
    WorldTextures textures;
    
    // Grid data
    Tile* tiles;
    ObjectType* objects;  // Grid of objects
    
    // Managers and systems
    struct ResourceManager* resourceManager;
    struct EntityPool* entityPool;
    struct MapSystem* mapSystem;
    struct ComponentRegistry registry;
} World;

// World state structure
typedef struct {
    World* world;
    struct EntityPool* entityPool;
    struct ComponentRegistry* registry;
    struct MapSystem* mapSystem;
    struct ResourceManager* textureManager;
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
void UpdateMapSystem(World* world, float deltaTime);
void DrawMapSystem(struct MapSystem* mapSystem);
void AddMapObject(struct MapSystem* mapSystem, ObjectType type, Vector2 position);
void RemoveMapObject(struct MapSystem* mapSystem, Vector2 position);
void UpdateMapObjects(struct MapSystem* mapSystem, float deltaTime);
void SaveMapSystem(struct MapSystem* mapSystem, const char* filename);
void LoadMapSystem(struct MapSystem* mapSystem, const char* filename);

// Tile management
void SetTile(World* world, int x, int y, TileType tileType);
TileType GetTile(World* world, int x, int y);
bool IsWalkable(const World* world, Vector2 position);
bool IsWalkableGrid(const World* world, int x, int y);

// Spawn point management
void AddSpawnPoint(World* world, Vector2 position);
Vector2 GetRandomSpawnPoint(World* world);

// Debug functions
void DrawWorldDebug(World* world);
void DrawCollisionDebug(World* world);

// World state management
World* CreateWorld(int width, int height, float gravity, struct ResourceManager* resourceManager);
void DestroyWorld(World* world);

// Tile management
void SetTileAt(World* world, int x, int y, Tile tile);
Tile GetTileAt(World* world, int x, int y);

void ClearWorld(World* world);

#endif // SHADOW_WORKER_WORLD_H