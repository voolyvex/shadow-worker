#ifndef MAP_SYSTEM_H
#define MAP_SYSTEM_H

#include <raylib.h>
#include "map_types.h"

#define MAX_LAYERS 5

// Forward declarations
struct TileMap;
struct CollisionGrid;
struct RenderLayer;

// Map System structure
typedef struct MapSystem {
    struct TileMap* currentMap;
    struct CollisionGrid* collisionGrid;
    struct RenderLayer* layers;
    int layerCount;
} MapSystem;

// Map system management functions
MapSystem* CreateMapSystem(void);
void DestroyMapSystem(MapSystem* mapSystem);
bool LoadMap(MapSystem* mapSystem, const char* fileName);
void UpdateMapState(MapSystem* mapSystem, float deltaTime);
void RenderMapLayers(MapSystem* mapSystem);

// Additional map system functions
void UpdateMapSystem(MapSystem* mapSystem, float deltaTime);
void DrawMapSystem(MapSystem* mapSystem);
void AddMapObject(MapSystem* mapSystem, ObjectType type, Vector2 position);
void RemoveMapObject(MapSystem* mapSystem, Vector2 position);
void UpdateMapObjects(MapSystem* mapSystem, float deltaTime);
void SaveMapSystem(MapSystem* mapSystem, const char* filename);
void LoadMapSystem(MapSystem* mapSystem, const char* filename);

#endif // MAP_SYSTEM_H 