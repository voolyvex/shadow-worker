#ifndef MAP_SYSTEM_H
#define MAP_SYSTEM_H

#include "map_types.h"
#include <raylib.h>

// Forward declarations
struct World;

// Map System structure
typedef struct MapSystem {
    TileMap* currentMap;
    CollisionGrid* collisionGrid;
    RenderLayer* layers;
    int layerCount;
} MapSystem;

// Map system management functions
MapSystem* CreateMapSystem(void);
void DestroyMapSystem(MapSystem* mapSystem);
bool LoadMap(MapSystem* mapSystem, const char* fileName);
void UpdateMapState(MapSystem* mapSystem, float deltaTime);
void RenderMapLayers(MapSystem* mapSystem);

// Additional map system functions
void UpdateMapSystem(struct World* world, float deltaTime);
void DrawMapSystem(MapSystem* mapSystem);
void AddMapObject(MapSystem* mapSystem, ObjectType type, Vector2 position);
void RemoveMapObject(MapSystem* mapSystem, Vector2 position);
void UpdateMapObjects(MapSystem* mapSystem, float deltaTime);
void SaveMapSystem(MapSystem* mapSystem, const char* filename);
void LoadMapSystem(MapSystem* mapSystem, const char* filename);

// Chunk management functions
CachedChunk* GetChunk(ChunkCache* cache, Vector2 gridPos);

#endif // MAP_SYSTEM_H 